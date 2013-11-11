<?php

/**
 * Implements hook_features_export().
 */
function campaign_features_export($data, &$export, $module_name = '', $type = 'campaign') {
  $pipe = array();
  drupal_set_message(t('If you want files to be exported you need to configure !link.', array(
    '!link' => l(t('node_export'), 'admin/config/content/node_export'),
  )));
  foreach ($data as $name) {
    if ($campaign = campaign_get($name)) {
      // We export nodes ourselves using function from node_export, but add the
      // node type to the pipe.
      foreach (array('prelike', 'postlike', 'inactive') as $page) {
        if (!empty($campaign['config'][$page])) {
          $path = $campaign['config'][$page];
          if ($source = drupal_lookup_path('source', $campaign['config'][$page])) {
            $path = $source;
          }
          if (preg_match('/^node\/([0-9]+)/', $path, $matches)) {
            $type = db_select('node', 'n')
              ->fields('n', array('type'))
              ->condition('nid', (int) $matches[1])
              ->execute()
              ->fetchField();
            $pipe['node'][$type] = $type;
          }
        }
      }
      // Export all nodes of a related content type.
      $content_types = array();
      foreach (explode(',', $campaign['config']['content_types']) as $type) {
        if ($type = trim($type)) {
          $pipe['node'][$type] = $type;
          $content_types[] = $type;
        }
      }
      foreach ($content_types as $type) {
        $uuids = db_select('node', 'n')
          ->fields('n', array('uuid'))
          ->condition('type', $type)
          ->execute()
          ->fetchCol();

        if (count($uuids) > 15) {
          drupal_set_message(t('Trying to export more than 15 nodes of type %type', array('%type' => $type)));
        }
        else {
          foreach($uuids as $uuid) {
            $pipe['node_export_features'][$uuid] = $uuid;
          }
        }
      }

      foreach (campaign_get_related_view_names($name) as $view) {
        $pipe['views_view'][$view] = $view;
      }
      foreach (campaign_get_related_context_names($name) as $context) {
        $pipe['context'][$context] = $context;
      }
      foreach (campaign_get_related_rules($name) as $id => $rule) {
        $pipe['rules_config'][$id] = $id;
      }

      $export['features']['campaign'][$name] = $name;
    }
  }

  $export['dependencies']['campaign'] = 'campaign';
  return $pipe;
}

/**
 * Implements hook_features_export_options().
 */
function campaign_features_export_options() {
  $options = array();
  foreach (campaign_list() as $name) {
    $options[$name] = $name;
  }
  return $options;
}

/**
 * Implements hook_features_revert().
 */
function campaign_features_revert($module) {
  campaign_features_rebuild($module);
}

/**
 * Implements hook_features_rebuild().
 */
function campaign_features_rebuild($module) {
  if ($feature_conf = features_get_default('campaign', $module)) {
    foreach ($feature_conf as $name => $config) {
      $context = (object) $config['context'];
      $campaign = &$context->reactions['campaign_context_reaction']['campaign'];
      // Import files
      foreach ($config['files'] as $idx => $image) {
        $file = (object) $image['file'];
        if (file_prepare_directory(dirname($file->uri), FILE_CREATE_DIRECTORY)) {
          if (file_put_contents($file->uri, base64_decode($image['data']))) {
            // Save as a new file.
            $file = file_save($file);
            // Update the file id to the new one.
            $campaign['blog_banner']['images'][$idx]['file'] = $file->fid;
          }
          else {
            drupal_set_message(t('Could not create file !file',  array('!file' => $file->uri)));
          }
        }
        else {
          drupal_set_message(t('Could not create directory !dir', array('!dir' => dirname($file->uri))));
        }
      }

      // Import nodes
      // @see node_export_features_features_rebuild
      if (!empty($config['node_export']['code_string'])) {
        // Import but dont save nodes yet, we need the new nids.
        $result = node_export_import($config['node_export']['code_string'], 't', FALSE);
        if (!$result['success']) {
          foreach ($result['output'] as $error) {
            drupal_set_message($error, 'error');
          }
        }
        else {
          $total = count($result['nodes']);
          $count = 0;
          foreach ($result['nodes'] as $node) {
            node_export_save($node);
            $original_nid = $config['uuid_map'][$node->uuid];
            foreach (array('prelike', 'postlike', 'inactive') as $page) {
              if ($campaign['config'][$page] == 'node/' . $original_nid) {
                // Update the nid reference.
                $campaign['config'][$page] = 'node/' . $node->nid;
                $result['output'][] = t('Mapped !page from !old to !new', array('!page' => $page, '!old' => $original_nid, '!new' => $node->nid));
              }
            }
            $result['output'][] = t('Imported node !nid: !node', array('!nid' => $node->nid, '!node' => l($node->title, 'node/' . $node->nid)));
            $count += 1;
          }
          drupal_alter('node_export_after_import', $new_nodes, $used_format, $save);
          $result['output'][] = t('!count of !total nodes were imported. Some values may have been reset depending on Node exports configuration.', array('!total' => $total, '!count' => $count));

          foreach ($result['output'] as $status) {
            drupal_set_message($status);
          }
        }
      }
    }

    // Delete the current campaign if it exists
    context_delete($context);
    context_save($context);
    cache_clear_all();
  }
}

/**
 * Implements hook_features_export_render().
 */
function campaign_features_export_render($module_name, $data, $export = NULL) {
  $code = array();
  $code[] = '  $config = array();';
  $code[] = '';
  foreach ($data as $key => $name) {
    if ($context = context_load($name)) {
      $campaign = $context->reactions['campaign_context_reaction']['campaign'];

      // Export blog banner files
      $files = array();
      if (!empty($campaign['blog_banner']['images'])) {
        foreach ($campaign['blog_banner']['images'] as $idx => $image) {
          $file = file_load($image['file']);
          $data = base64_encode(file_get_contents($file->uri));
          $files[$idx] = array(
            'data' => $data,
            'file' => $file,
          );
        }
      }

      // Export nodes mentioned.
      $uuids = array();
      $nids = array();
      $uuid_map = array();
      foreach (array('prelike', 'postlike', 'inactive') as $page) {
        if (!empty($campaign['config'][$page])) {
          $path = $campaign['config'][$page];
          if ($source = drupal_lookup_path('source', $campaign['config'][$page])) {
            $path = $source;
          }
          if (preg_match('/^node\/([0-9]+)/', $path, $matches)) {
            $nid = $matches[1];
            $uuid = uuid_get_uuid('node', 'nid', $nid);
            $uuids[] = $uuid;
            $nids[] = $nid;
            $uuid_map[$uuid] = $nid;
          }
        }
      }
      // @see node_export_features_features_export_render()
      $result = node_export($nids);
      if ($result['success']) {
        $node_export['code_string'] = $result['output'];
        $node_export_code = features_var_export($node_export);
      }
      else {
        foreach ($result['output'] as $error) {
          $node_export_code  = '  // ' . $error . PHP_EOL;
        }
        $node_export_code .= 'array()';
      }

      $context_export_code = features_var_export($context);
      $files_export_code = features_var_export($files);
      $uuid_map_code = features_var_export($uuid_map);
      $code[] = "  // Exported campaign {$name}";
      $code[] = "  \$config['{$key}']['context'] = {$context_export_code};";
      $code[] = "  \$config['{$key}']['files'] = {$files_export_code};";
      $code[] = "  \$config['{$key}']['node_export'] = {$node_export_code};";
      $code[] = "  \$config['{$key}']['uuid_map'] = {$uuid_map_code};";
      $code[] = "";

    }
  }
  $code[] = '  return $config;';
  $code = implode("\n", $code);
  return array('campaign_export_defaults' => $code);
}

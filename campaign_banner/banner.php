<?php

// @TODO can we figure out correct path without bootstrapping drupal?
define('DRUPAL_ROOT', realpath(__DIR__ . '/../../../../../..'));

// Read $databases from drupal settings.
require_once DRUPAL_ROOT . '/sites/default/settings.php';
// Read CAMPAIGN_BANNER_UPLOAD_DIR, CAMPAIGN_BANNER_ID
require_once __DIR__ . '/campaign_banner.module';

class DB {
  public $db;

  protected $table = "campaign_counter";

  function __construct($obj) {
    $user = $obj['default']['default']['username'];
    $pass = $obj['default']['default']['password'];
    $host = $obj['default']['default']['host'];
    $path = $obj['default']['default']['database'];

    $this->db = new PDO("mysql:host=$host;dbname=$path", $user, $pass);
  }

  /**
   * @see campaign_counter_increment().
   */
  function increment($ref, $campaign) {
    $sth = $this->db->prepare("INSERT INTO $this->table (hits, ref, campaign, type, updated) VALUES (1, :ref, :campaign, :type, UNIX_TIMESTAMP()) ON DUPLICATE KEY UPDATE hits = hits + 1");
    return $sth->execute(array(
      ':ref' => $ref,
      ':campaign' => $campaign,
      ':type' => CAMPAIGN_BANNER_ID,
    ));
  }
}

$image = isset($_GET['image']) ? $_GET['image'] : NULL;
$campaign = isset($_GET['campaign']) ? $_GET['campaign'] : NULL;
$ref = isset($_GET['ref']) ? $_GET['ref'] : NULL;

if (!isset($image) || !isset($campaign)) {
  exit();
}
// The relative path to the uploads directory
$upload_dir = str_replace('public://', DRUPAL_ROOT . '/sites/default/files/', CAMPAIGN_BANNER_UPLOAD_DIR) . $campaign;
$extension = pathinfo($image, PATHINFO_EXTENSION);
$image = $upload_dir . '/' . $image;
$mimetypes = array(
  'png' => 'image/png',
  'jpg' => 'image/jpeg',
  'gif' => 'image/gif',
);

// Stop varnish from cashing.
if (!isset($_COOKIE['NO_CACHE'])) {
  setcookie('NO_CACHE', 'Y', $_SERVER['REQUEST_TIME'] + 300, $_SERVER['REQUEST_URI'], $_SERVER['SERVER_NAME']);
}

if (in_array($extension, array_keys($mimetypes)) && file_exists($image)) {
  if (isset($ref)) {
    $db = new DB($databases);
    $db->increment($ref, $campaign);
  }

  header('Content-Type: ' . $mimetypes[$extension]);
  header('Content-Length: ' . filesize($image)); // optimization
  $f = fopen($image, 'rb');
  fpassthru($f);
  fclose($f);
}
else {
  header('HTTP/1.0 404 Not Found');
  echo 'File not found';
}

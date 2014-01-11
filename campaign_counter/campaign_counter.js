(function($) {
  Drupal.campaign = Drupal.campaign || {};

  /**
   * Table of counter functions keyed by the counter type. Each function
   * should return a $.Deferred() object which always resolves to a count,
   * if an error occured, it should resolve to 0.
   *
   * @see Drupal.campaign.counters.facebook().
   */
  Drupal.campaign.counters = Drupal.campaign.counters || {};

  /**
   * Return a promise which resolves into the count value and defaulting to 0
   * on failure. This means the promise always resolves so it can be used even
   * though one counter fails.
   *
   * @param string type
   *   The counter type
   * @param string ref
   *   The counter reference (optional).If no reference is used the amount of
   *   ongoing counters with specified $type is returned.
   * @returns promise
   *
   * Usage:
   *   var twitter = Drupal.campaign.count('tweetcounter', 'jsgit')
   *     , fb = Drupal.campaign.count('facebook', 'mushbarf.fi')
   *     , banner = Drupal.campaign.count('campaign_banner')
   *     , qr = Drupal.campaign.count('qr');
   *
   *   $.when(fb, twitter, banner, qr)
   *     .done(function() {
   *       var total = 0;
   *       $.each(arguments, function (idx, obj) { total += obj[0]; });
   *       console.log(total);
   *     });
   */
  Drupal.campaign.count = function(type, ref) {
    var campaign = Drupal.settings.campaign.namespace
      , $deferred;

    // If there is a custom counter use it.
    if (type in Drupal.campaign.counters) {
      $deferred = Drupal.campaign.counters[type](ref);
    }
    // Otherwise fallback to querying the counter table.
    else {
      var path = [location.origin, 'campaign_counter/ajax']
        , method = ref ? 'hits' : 'unique';

      $deferred = $.Deferred();

      path.push(method);
      path.push(campaign);
      path.push(type);
      // If a reference is specified, we want the hits column.
      if (ref) path.push(ref);
      path = path.join('/');

      $.getJSON(path)
        .done(function(data) {
          $deferred.resolve(data.result, 'success');
        })
        .fail(function(data) {
          if (console && console.log) console.log('Error fetching: ' + path, arguments);
          $deferred.resolve(0, 'error');
        });
    }

    return $deferred.promise();
  };
}(jQuery));

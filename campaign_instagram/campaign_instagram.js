(function($) {
  Drupal.campaign.counters = Drupal.campaign.counters || {};

  /**
   * Hook into campaign_counter to provide a facebook like counter
   */
  Drupal.campaign.counters.instagram = function() {
    var namespace = Drupal.settings.campaign.namespace
      , $deferred = $.Deferred()
      , path = location.origin + '/campaign_instagram/ajax/' + namespace;

    $.getJSON(path)
      .done(function(data) {
        $deferred.resolve([data.result, 'success']);
      })
      .fail(function(data) {
        console && console.log && console.log('Error fetching: ' + path);
        $deferred.resolve([0, 'error']);
      });

    return $deferred;
  }
}(jQuery));

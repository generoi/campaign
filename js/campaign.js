(function($) {
  var isFacebook = false
    , isInit = false;

  // Sniff facebook.
  if (/^(app_runner|iframe_canvas)/.test(window.name || '')) {
    $('html').addClass('facebook');
    isFacebook = true; // Drupal.settings doesn't exist yet.
  }

  // socialite.facebook.js in the social module takes care of initializing
  // the Facebook SDK through fbAsyncInit(). Once it has been initalized,
  // the fb.init event is published with the help of jquery-tiny-pubsub.
  $.subscribe('fb.init', function() {
    isInit = true;

    if (isFacebook && FB.Canvas) {
      FB.Canvas.setAutoGrow();
      FB.Canvas.scrollTo(0, 0);
    }
  });

  // Load facebook network even on pages with widgets.
  if (!Socialite.networkReady('facebook')) {
    Socialite.appendNetwork(Socialite.networks.facebook);
  }

  Drupal.behaviors.campaign = {
    attach: function(context, settings) {
      Drupal.campaign.attachEvents();
      if (isFacebook) Drupal.campaign.executeQueue();
    }
  };

  Drupal.campaign = Drupal.campaign = {};

  /**
   * Listen to click events on fb-event elements. This works by adding a DOM
   * element with the class 'fb-event' as well ass all FB.ui properties as
   * data-attributes.
   *
   * Example:
   * <button class="fb-event" data-method="feed" data-link="http://www.google.com" data-description="Desc">Feed</button>
   * <button class="fb-event" data-method="apprequests" data-message="My message" data-title="Title">App request</button>
   * <button class="fb-event" data-method="send" data-link="http://www.google.com" data-description="Desc">Desc</button>
   */
  Drupal.campaign.attachEvents = function() {
    $('body').once('dialog').on('click', '.fb-event', function() {
      var $this = $(this);
      Drupal.campaign.run(function() {
        this.ui($this.data());
      });
    });
  };

 /**
  * Execute queued dialogs added through Drupal.
  */
  Drupal.campaign.executeQueue = function() {
    var dialogs = window.dialogs || [];
    if (Drupal.settings.campaign && Drupal.settings.campaign.dialogs) {
      dialogs = dialogs.concat(Drupal.settings.campaign.dialogs);
      // Empty the list not to prompt the user on possible AJAX requests.
      settings.dialogs = [];
    }
    for (var i = 0, l = dialogs.length; i < l; i++) {
      var obj = dialogs[i];
      Drupal.campaign.run(function() {
        this.ui(obj);
      });
    }
    window.dialogs = [];
  };

  /**
   * Make sure the callback runs after FB has been inited.
   *
   * Usage:
   * Drupal.campaign.run(function() { this.ui({}); });
   */
  Drupal.campaign.run = function(callback) {
    if (isInit) return callback.call(FB);
    $.subscribe('fb.init', function() {
      callback.call(FB);
    });
  };

  /**
   * Transform a webform by placing component descriptions as input
   * placeholders.
   *
   * Usage:
   * Drupal.campaign.usePlaceholder($('.webform-client-form'));
   */
  Drupal.campaign.usePlaceholder = function($el) {
    // Default to webform.
    if (!$el) $el = $('.webform-client-form');

    // @TODO what about checkboxes and radios?
    $el.find('.description').each(function() {
      var $this = $(this).hide()
        , text = $this.text()
        , $input = $this.prev('input');

      $input.prop('placeholder', text);
      if ($.fn.placeholder) $input.placeholder();
    });
  };

}(jQuery));

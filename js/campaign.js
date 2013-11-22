(function($) {
  var isFacebook = false
    , isInit = false
    , log = function() { return console && console.log && Function.apply.call(console.log, console, arguments); };

  // Sniff facebook.
  if (/^(app_runner|iframe_canvas)/.test(window.name || '')) {
    $('html').addClass('facebook');
    isFacebook = true; // Drupal.settings doesn't exist yet.
  } else {
    $('html').addClass('not-facebook');
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

  Drupal.behaviors.campaign = {
    attach: function(context, settings) {
      Drupal.campaign.attachEvents();
      Drupal.campaign.executeQueue();

      if (settings.campaign && settings.campaign.force_facebook) {
        Drupal.campaign.forceFacebook();
      }
    }
  };

  Drupal.campaign = Drupal.campaign || {};

  // Contains custom counter hooks
  Drupal.campaign.counters = Drupal.campaign.counters || {};

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
      if (isFacebook) {
        Drupal.campaign.run(function() {
          this.ui(obj);
        });
      }
      console && console.log && console.log('Display dialog', obj);
    }
    window.dialogs = [];
  };

  /**
   * If the user is signed into Facebook but not viewing the site through
   * facebook, redirect them to the page tab.
   */
  Drupal.campaign.forceFacebook = function() {
    // Only run if we are not on facebook, and we arent using a mobile browser.
    if (!$('html').hasClass('not-facebook') || $(window).width() <= 810) {
      return;
    }
    Drupal.campaign.run(function() {
      this.getLoginStatus(function(response) {
        // We are signed into facebook.
        if (response.status !== 'unknown') {
          var page_tab = Drupal.settings.campaign.facebook_tab;
          if (page_tab) window.location.href = page_tab;
        }
      });
    });
  }

  function updateQueryStringParameters(uri, params) {
    for (var key in params) if (params.hasOwnProperty(key)) {
      var re = new RegExp('([?|&])' + key + '=.*?(&|$)', 'i')
        , separator = uri.indexOf('?') !== -1 ? '&' : '?';
      if (uri.match(re)) {
        uri = uri.replace(re, '$1' + key + '=' + params[key] + '$2');
      }
      else {
        uri = uri + separator + key + '=' + params[key];
      }
    }
    return uri;
  }

  /**
   * Alter the socialite facebook like buttons by adding UTM parameters.
   */
  Drupal.campaign.setUTM = function(e, attributes) {
    // Only active on campaign pages.
    if (!(Drupal.settings && Drupal.settings.campaign && Drupal.settings.campaign.namespace)) {
      return;
    }
    var params = {
      utm_source: 'facebook',
      utm_medium: 'like',
    };
    if (campaign = Drupal.settings.campaign && Drupal.settings.campaign.namespace) {
      params.utm_campaign = campaign;
    }

    attributes.href = updateQueryStringParameters(attributes.href, params);
  }

  // Currently only for facebook likes!
  $.subscribe('socialite.alterAttributes', Drupal.campaign.setUTM);

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

    // @TODO textarea not tested.
    $el.find('input + .description').each(function() {
      var $this = $(this).hide()
        , text = $this.text()
        , $input = $this.prev('input');

      $input.prop('placeholder', text);
      if ($.fn.placeholder) $input.placeholder();
    });
  };

  /**
   * Hook into campaign_counter to provide a facebook like counter
   */
  Drupal.campaign.counters.facebook = function(page) {
    var $deferred = $.Deferred();
    $.getJSON('https://graph.facebook.com/' + page)
      .done(function(data) {
        $deferred.resolve([data.likes, 'success']);
      })
      .fail(function(data) {
        $deferred.resolve([0, 'error']);
      });
    return $deferred;
  }

}(jQuery));

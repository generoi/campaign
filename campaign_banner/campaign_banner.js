(function($) {
  Drupal.behaviors.campaignBanner = {
    attach: function (context) {
      $('.banner-generator-wrapper').once(function () {
        Drupal.campaignBanner.attachListeners(this, $(this).data());
      });
    }
  };

  Drupal.campaignBanner = Drupal.campaignBanner || {};

  Drupal.campaignBanner.attachListeners = function(el, data) {
    var $el = $(el),
      $output = $el.find('textarea').bind('focus', forceSelect);

    $el.on('click', '.banner-generator', data, function(event) {
      Drupal.campaignBanner.generateBanner.call(this, event, $output);
    });
  }

  Drupal.campaignBanner.generateBanner = function(event, $output) {
    var ref = clean(window.prompt(Drupal.t('Enter the URL of your blog'), 'http://')),
      filename = $(this).find('img').prop('src'),
      image = filename + '&ref=' + window.encodeURIComponent(ref),
      href = event.data.href + '?utm_source=' + window.encodeURIComponent(ref),
      alt = event.data.alt,
      snippet = Drupal.campaignBanner.generate(href, image, alt);

    if (!ref) snippet = Drupal.t('Invalid URL.');
    $output.val(snippet).select();
    $('html, body').animate({
      scrollTop: $output.offset().top
    }, 500);
    event.preventDefault();
  }

  Drupal.campaignBanner.generate = function (href, image, alt) {
    return [
      '<a href="' + href + '">',
        '<img src="' + image + '" alt="' + alt + '" />',
      '</a>'
    ].join('');
  };

  function forceSelect() {
    var $this = $(this);
    $this.select();
    $this.mouseup(function() {
      $this.unbind('mouseup');
      return false;
    });
  }


  function clean(url) {
    var match = url.match(/^(?:https?:\/\/)?((?:\w+\.){1,2}\w+(?:\/\w+(?!\.))?).*/);
    if (match) return match[1];
    return false;
  }
  // Test regexp
  // var regexp = /^(?:https?:\/\/)?((?:\w+\.){1,2}\w+(?:\/\w+(?!.))?).*/;
  // ['http://www.google.com'
  //   ,'https://www.google.com'
  //   ,'http://google.com'
  //   ,'http://www.google.com/path'
  //   ,'http://www.google.com/path.php'
  //   ,'www.google.com/path.php'
  // ].forEach(function(url) { console.log(url.match(regexp)); });
}(jQuery));

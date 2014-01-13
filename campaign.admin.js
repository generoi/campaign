(function ($) {
  Drupal.behaviors.campaignUI = {
    attach: function(context) {
      $(context).find('table.campaign-config-overview').on('click', 'a.toggle-details', function(event) {
         var $this = $(this).toggleClass('open');
         $this.parent('div').next('.campaign-config-details').slideToggle();
         event.stopPropagation();
      });
    }
  };
}(jQuery));

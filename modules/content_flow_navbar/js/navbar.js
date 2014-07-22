(function ($) {

Drupal.navbar = Drupal.navbar || {};

/**
 * Attach toggling behavior and notify the overlay of the navbar.
 */
Drupal.behaviors.navbar = {
  attach: function(context) {
    $('body').css('paddingTop', Drupal.navbar.height());
  }
};

Drupal.navbar.height = function() {
  var $navbar = $('#bootstrap-admin-navbar .navbar');
  var height = $navbar.outerHeight();
  // In modern browsers (including IE9), when box-shadow is defined, use the
  // normal height.
  var cssBoxShadowValue = $navbar.css('box-shadow');
  var boxShadow = (typeof cssBoxShadowValue !== 'undefined' && cssBoxShadowValue !== 'none');
  // In IE8 and below, we use the shadow filter to apply box-shadow styles to
  // the navbar. It adds some extra height that we need to remove.
  if (!boxShadow && /DXImageTransform\.Microsoft\.Shadow/.test($navbar.css('filter'))) {
    height -= $navbar[0].filters.item("DXImageTransform.Microsoft.Shadow").strength;
  }
  return height;
};

})(jQuery);

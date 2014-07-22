<?php

/**
 * @file
 * Default template for admin navbar.
 *
 * Available variables:
 * - $classes: String of classes that can be used to style contextually through
 *   CSS. It can be manipulated through the variable $classes_array from
 *   preprocess functions. The default value has the following:
 *   - navbar: The current template type, i.e., "theming hook".
 * - $navbar['navbar_user']: User account / logout links.
 * - $navbar['navbar_menu']: Top level management menu links.
 * - $navbar['navbar_drawer']: A place for extended navbar content.
 *
 * Other variables:
 * - $classes_array: Array of html class attribute values. It is flattened
 *   into a string within the variable $classes.
 *
 * @see template_preprocess()
 * @see template_preprocess_navbar()
 *
 * @ingroup themeable
 */
?>
<div id="bootstrap-admin-navbar">
  <div class="navbar navbar-inverse navbar-fixed-top overlay-displace-top" role="navigation">
    <div class="container-fluid">
      <div class="navbar-header">
        <button type="button" class="navbar-toggle" data-toggle="collapse" data-target=".navbar-collapse">
          <span class="sr-only">Toggle navigation</span>
          <span class="icon-bar"></span>
          <span class="icon-bar"></span>
          <span class="icon-bar"></span>
        </button>
        <?php print render($navbar['navbar_logo']); ?>
        <?php print render($navbar['navbar_home']); ?>
      </div>
      <div class="collapse navbar-collapse">
        <?php print render($navbar['navbar_main']); ?>
        <?php print render($navbar['navbar_right']); ?>
      </div><!--/.nav-collapse -->
    </div>
  </div>
</div>

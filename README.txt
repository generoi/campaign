Campaign
========

This module organizes Facebook campaigns running on Drupal in a streamlined
way. It includes a couple of components which are designed to be used together
with the modularity of Drupal

1. A performance oriented gateway which redirects user to specified URLs
depending on if they have liked a facebook page. Additionally it serves
post-like pages to non-JavaScript browsers (googlebot) and adds JavaScript
redirection to the appropriate Facebook URL depending on how they arrived.
2. Context integration for activating a Facebook campaign with the JavaScript
SDK. This will also initalize the Canvas by scrolling to the top and resizing
the page height etc.
3. Rules components. Currently an action to display a Facebook share dialog.

The main concept is to:

1. Easily initialize Facebook campaigns depending on the page.
2. Create custom before and after pages with regular nodes.
3. Display share dialogs with rules.
4. Make the entire Drupal page Facebook friendly (the theme is required to
style the page apprioriately when the facebook class exists on <html>)
5. Integrate with all things Drupal all be cachable by varnish (everything
except for the gateway).

Installation
------------
1. Install like any other Drupal module
2. Grant the 'Administer campaigns' permission to relevant roles.
3. Visit admin/config/campaigns and create your first campaign.

Requirements
------------
- http://drupal.org/project/underscore
- http://drupal.org/project/context
- The social module developed by Genero (this is primarily needed for
jquery-tiny-pubsub and it's fbAsyncInit management).
- http://drupal.org/project/easy_social is a soft dependency of social

FAQ
---
Q: How do I make this work with Varnish?
A: @TODO, don't cache ^/(sv|en|fi)/gateway/.*$

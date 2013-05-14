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

Howto configure a campagin
--------------------------
1. Create a before-like page. Content > Add content
   - Enter the content you want to display for users when they haven't liked
     the facebook page.
   - Set a path manually, just so it doesn't take up an important alias, this
     page should not be reached by users.
   - Configure meta tags, these will be displayed when the campaign is linked
     on Facebook
     - Set the Open Graph tags, these are the ones showed on Facebook.
       - Do NOT set the Facebook Application ID.
     - The regular meta tags will be displayed on Google

2. Create after-like page. Usually this is a webform. If you want to use the
   regular node add pages, you don't need to create this page. The same
   situation if you already have an existing page.
   - Remember to set the path, this will be the path used when non-facebook
     users participate.
   - Set the Open Graph tags to the same as the before like page.

3. Visit https://developers.facebook.com/apps/ and create your app, choose the
   namespace carefully as this will be used in the path when sharing the
   campaign.
   - Do not set the App domain yet, simply disable sandbox mode and save.

4. Now visit the campaign administration page at admin/config/campaigns and
   selecting "Add a campaign by path". In the first field enter the
   Application ID which you get from the facbook app page.

6. Configure your campaign.
   - Path, these are the pages where the JS SDK will be initalized, it should
     at least be the before- and after-like pages.
   - Enter the Facebook Application secret and the Tab URL. To get a Tab URL
     you should add the Application to your Facebook page.
     Simply click the "adding it to your page"-link in the field description
     and once added, navigate to the page and the newly created tab
     application. Copy the current URL and paste it into the configuration
     field.
   - Configure the remaining parts of the application and click Save.

7. Click the campaign name to expand the data related to it. Copy and paste
   the "App / Canvas page" URL, the "Page Tab" URL and the "Mobile Web" URL
   into the Facebook app administration. (Hint, you can access the app
   administration by clicking the 'Developers' link).

8. Alright now your app is set up.

Create a sample campaign
------------------------
Lets create create a simple campaign which checks a users answer to a webform
question and if correct, randomly selects X winners per day.

1. Enable the rules_random and the webform_rules modules.

2. Configure the webform node with the necessary components.
   - Note the machine name of the question component (e.g. "question").
   - If you want to use select options, take note of the keys, e.g. my
     numbering them 1-3, with the winner being 2.

3. Add an additional textfield-component to contain the correct question
   value.
   - Set the default value to the key of the select alternative
   - Make the component inactive (NOT private!).
   - Also hide the label.

4. Finally go to "Form settings"
   - Choose "No redirect" (if you do a redirect, should remember to add the new
     page to the campaign paths).
   - Disable the "Show notification about previous submissions".

5. Now add the required rules. Visit admin/config/workflow/rules

   1. Create a rule listening to "After a webform has been submitted" and tag
      it with the machine name of the campaign (this will list the rule in the
      campaign overview).

   2. Add a "Webform has name"-condition.
      - Value field: "[form-id:value]"
      - Webforms: select the webforms (if webforms use the same component
        machine names you can select multiple).

   3. Add a "webform question comparison"-condition.
      - Data selector should be set to "data".
      - Question field key: the machine name of the question webform component
        (eg. question).
      - Solution field key

   4. Add a "Limited randomize"-condition.
      - Identifier: for example node:nid
      - Database value: For example the user email, "[data:email-value]", if
        "email" is the machine name of the component.
      - Probability: Set it 100 for initial testing and lower it to 2-10 when
        going live, depending on how many submissions you expect.
      - Hours of draw: The hours a winner will be drawn, set one to the current
        hour so you can test it.

   5. Now the conditions are set, next up is actions. Add add a "Display a feed
      dialog"-action.
      - Image: the path of the image you used in the opengraph data for the
        before like page.
      - Link path: The "Redirect URL" mentioned in the campaign overview, this
        is http://<domain>/<lang>/fb/<namespace>
      - Name: "I won ..."
      - Description: "Try your luck as well"

    6. You might also want to add en email action which notifies the winner of
       what will happen next.

    7. Next up you should clone rule to create the Correct but not winner
       alternative.
       - Selet the "Limited randomize"-condition and use the Negate-option.
       - Configure the Feed dialog.
       - Remember! You have to MANUALLY sync the "Hours of draw" of the winner
         and not winner rules.

    8. Alright the last alternative is Incorrect answer. Clone the rule again.
       - Delete the "Limited randomize"-condition.
       - Negate the "Webform question comparison"-condition.
       - Configure the Feed dialog.


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

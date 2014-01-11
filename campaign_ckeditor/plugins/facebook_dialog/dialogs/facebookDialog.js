(function(CKEDITOR) {
  CKEDITOR.dialog.add('facebookDialog', function (editor) {
    var partIds = ['feedOptions', 'sendOptions', 'apprequestsOptions']
      , defaultOptions = 'feed';

    function dialogTypeChange() {
      var dialog = this.getDialog()
        , typeValue = this.getValue();

      for (var i = 0, l = partIds.length; i < l; i++) {
        var element = dialog.getContentElement('info', partIds[i]);
        if (!element) continue;
        element = element.getElement().getParent().getParent();
        if (partIds[i] === typeValue + 'Options') element.show();
        else element.hide();
      }
      dialog.layout();
    }

    function createTextField(type, id, label, defaultText) {
      return {
        type: 'text',
        label: label,
        id: type + '-' + id,
        'default': defaultText || '',
        setup: function(element) {
          if (id === 'button') {
            this.setValue(element.getText());
          } else {
            this.setValue(element.getAttribute('data-' + id));
          }
        },
        commit: function(data) {
          if (!data[type]) data[type] = {};
          data[type][id] = this.getValue();
        }
      };
    }

    function displayOptions(type) {
      return function(element) {
        if (element.getAttribute('data-method') === type) {
          this.getElement().show();
        } else {
          this.getElement().hide();
        }
      };
    }

    return {
      title: 'Facebook Dialog Properties',
      minWidth: 400,
      minHeight: 200,
      contents: [
        {
          id: 'info',
          label: 'Properties',
          title: 'Properties',
          elements: [{
            id: 'dialogType',
            type: 'select',
            label: 'Dialog type',
            'default': defaultOptions,
            items: [
              ['Send', 'send'],
              ['Request', 'apprequests'],
              ['Feed', 'feed']
            ],
            onChange: dialogTypeChange,
            setup: function(element) {
              var type = element.getAttribute('data-method');
              if (type) this.setValue(type);
            },
            commit: function(data) { data.type = this.getValue(); }
          }, {
            type: 'vbox',
            id: 'sendOptions',
            children: [
              createTextField('send', 'picture', 'Picture'),
              createTextField('send', 'link', 'Link'),
              createTextField('send', 'name', 'Name'),
              createTextField('send', 'caption', 'Caption'),
              createTextField('send', 'description', 'Description'),
              createTextField('send', 'button', 'Button text', 'Send')
            ],
            setup: displayOptions('send')
          }, {
            type: 'vbox',
            id: 'feedOptions',
            children: [
              createTextField('feed', 'picture', 'Picture'),
              createTextField('feed', 'link', 'Link'),
              createTextField('feed', 'name', 'Name'),
              createTextField('feed', 'to', 'To'),
              createTextField('feed', 'description', 'Description'),
              createTextField('feed', 'button', 'Button text', 'Feed')
            ],
            setup: displayOptions('feed')
          }, {
            type: 'vbox',
            id: 'apprequestsOptions',
            children: [
              createTextField('apprequests', 'title', 'Title'),
              createTextField('apprequests', 'message', 'Message'),
              createTextField('apprequests', 'max_recipients', 'Max Recipients'),
              createTextField('apprequests', 'to', 'To'),
              createTextField('apprequests', 'button', 'Button text', 'Request')
            ],
            setup: displayOptions('apprequests')
          }]
        }
      ],
      onLoad: function(data) {
        for (var i = 0, l = partIds.length; i < l; i++) {
          if (partIds[i] !== defaultOptions + 'Options') {
            var element = this.getContentElement('info', partIds[i]);
            if (!element) continue;
            element = element.getElement().getParent().getParent();
            element.hide();
          }
        }
        this.layout();
      },
      onShow: function() {
        var selection = editor.getSelection()
          , element = selection.getStartElement();

        if (element) element = element.getAscendant('button', true);
        if (!element || !element.hasClass('fb-event')) {
          element = editor.document.createElement('button');
          element.setAttribute('class', 'fb-event');
          this.insertMode = true;
        } else {
          this.insertMode = false;
        }
        this.element = element;
        if (!this.insertMode) {
          this.setupContent(this.element);
        }
      },
      onOk: function() {
        var dialog = this
          , button = this.element
          , data = {};

        this.commitContent(data);
        button.setAttribute('class', 'fb-event');
        button.setAttribute('data-method', data.type);
        for (var key in data[data.type]) if (data[data.type].hasOwnProperty(key)) {
          var value = data[data.type][key];
          if (value.length) {
            if (key === 'button') {
              button.setText(value);
            } else {
              button.setAttribute('data-' + key, value);
            }
          }
        }

        if (this.insertMode) {
          editor.insertElement(button);
        }
      }
    };
  });
}(window.CKEDITOR));

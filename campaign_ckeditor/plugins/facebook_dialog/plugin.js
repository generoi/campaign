(function(CKEDITOR) {
  CKEDITOR.plugins.add('facebookDialog', {
    requires: 'dialog,fakeobjects',
    init: function(editor) {
      editor.addCommand('facebookDialog', new CKEDITOR.dialogCommand('facebookDialog', {
        allowedContent: 'button[data-picture,data-link,data-name,data-caption,data-description,data-to,data-max_recipients,data-title,data-message,data-method]{*}(*)',
        requiredContent: 'button'
      }));
      editor.ui.addButton('FacebookDialog', {
        label: 'Add Facebook dialog button',
        command: 'facebookDialog',
        icon: this.path + 'icon.jpg'
      });

      if (editor.contextMenu) {
        editor.addMenuGroup('facebookDialogGroup');
        editor.addMenuItem('facebookDialogItem', {
          label: 'Edit Facebook Dialog',
          icon: this.path + 'icon.jpg',
          command: 'facebookDialog',
          group: 'facebookDialogGroup'
        });
        editor.contextMenu.addListener(function(element) {
          if (element.getAscendant('button', true) && element.hasClass('fb-event')) {
            return { facebookDialogItem: CKEDITOR.TRISTATE_OFF };
          }
        });
      }
      CKEDITOR.dialog.add('facebookDialog', this.path + 'dialogs/facebookDialog.js');
    }
  });

}(window.CKEDITOR));

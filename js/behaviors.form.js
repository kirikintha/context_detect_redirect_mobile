/**
 * Module jQuery functions and Miscellaneous functions specific to this module.
 */

//If javascript is enabled
if (Drupal.jsEnabled) {
  //Variables
  var target;
  //Set module namespace.
  Drupal.contextConditionDetectMobile       = Drupal.contextConditionDetectMobile || {};
  Drupal.contextConditionDetectMobile.theme = Drupal.contextConditionDetectMobile.theme || {};
  //Bind page events.
  Drupal.behaviors.contextConditionDetectMobileInit = function(context) {
    if ($('.context-condition-detect-mobile-options-form-devices').length) {
      Drupal.contextConditionDetectMobile.setStatus();
    }
    if ($('.context-condition-detect-mobile-options-form-devices .form-item input.form-checkbox').length) {
      $('.context-condition-detect-mobile-options-form-devices input.form-checkbox', context).each(function() {
        $(this).bind('click', Drupal.contextConditionDetectMobile.updateStatus);
      });
    }
  }
  //Set status of all items.
  Drupal.contextConditionDetectMobile.setStatus = function() {
    $('.context-condition-detect-mobile-options-form-devices .form-item').each(function() {
      if ($(this).find('input.form-checkbox').is(':checked') === true) {
        $(this).addClass('ok');
      } else {
        $(this).addClass('warning');
      }
    });
  };
  //Set status of an individual item.
  Drupal.contextConditionDetectMobile.updateStatus = function() {
    target = $(this).parents('.form-item');
    targetID = target.attr('id');
    $('#'+targetID).removeClass('ok').removeClass('warning');
    if ($(this).is(':checked') === true) {
      $('#'+targetID).addClass('ok');
    } else {
      $('#'+targetID).addClass('warning');
    }
  };
}
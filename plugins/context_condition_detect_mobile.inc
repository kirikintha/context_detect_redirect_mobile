<?php
// $Id: context_reaction_detect_mobile.inc,v 1.1.2.2 2010/10/25 18:47:40 brynbellomy Exp $

/**
 * Expose themes as context reactions.
 */
class context_condition_detect_mobile extends context_condition {
  /**
   * Allow admins to choose the theme to be set.
   */
  function options_form($context) {
    //Radio Options. Shows a list of devices to ignore detection.
    $devices = array();
    if (module_exists('wurfl')) {
      $query  = db_query("SELECT did, id, brand_name, model_name, capabilities FROM {wurfl_devices}
        WHERE model_name != ''
        GROUP BY model_name
        ORDER BY id, model_name ASC
      ");
      //Lopo through devices, and determine if it should be allowed in this list. If it is a wireless device, has HTML support or is a touch screen or is a tablet then we add it into this list.
      while ($device = db_fetch_object($query)) {
        $capabilities = unserialize($device->capabilities);
        if ($capabilities['product_info']['is_tablet'] || $capabilities['product_info']['is_wireless_device'] || $capabilities['product_info']['pointing_method'] || $capabilities['product_info']['device_claims_web_support']) {
          $devices[$device->id] = t($device->model_name);
        }
      }
    }
    //If we have empty devices, then something has been set wrong. set a drupal message.
    if (empty($devices)) {
      $MSG = 'There are no devices to detect, please make sure that you have updated WURFL and make a test.';
      drupal_set_message(t($MSG), 'error');
    }
    //Form.
    $form = array(
      '#tree'    => TRUE,
      '#title'   => t('Mobile: Detection'),
      'detect_mobile' => array(
        '#title'         => t('Exclusion List.'),
        '#description'   => t('To ignore a device, check the box next to it.'),
        '#type'          => 'checkboxes',
        '#options'       => $devices,
        '#default_value' => isset($values['detect_mobile']) ? $values['detect_mobile'] : array(),
      ),
    );
    return $form;
  }

  /**
   * Set the active theme.
   */
  function execute() {
    foreach (context_active_contexts() as $context) {
      if (isset($context->reactions[$this->plugin]['detect_mobile'])) {
        //If we have our device detection, then let's look at it.
        
      }
    }
  }
}
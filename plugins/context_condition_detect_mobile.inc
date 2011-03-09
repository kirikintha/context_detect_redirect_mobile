<?php
/**
 * Expose mobile detection as a context condition.
 */
class context_condition_detect_mobile extends context_condition {
  /**
   * Omit condition values. We will provide a custom input form for our conditions.
   */
  function condition_values() {
    return array(
     1 => t('Enable Mobile Detection'),
    );
  }
  /**
   * Override of condition_form().
   * Create our mobile devices form with grouped checkboxes.
   */
  function condition_form($context) {
    $form = parent::condition_form($context);
    $form['#default_value'] = array(1);
    return $form;
  }

   /**
   * Override of condition_form().
   * Create our mobile devices form with grouped checkboxes.
   */
  function options_form($context) {
    drupal_add_css(CONTEXT_DETECT_REDIRECT_MOBILE_MODULE_PATH .'/css/admin-styles.css');
    drupal_add_js(CONTEXT_DETECT_REDIRECT_MOBILE_MODULE_PATH .'/js/behaviors.form.js');
    // Start Form.
    $form = array(
      '#tree'    => TRUE,
      '#title'   => t('Mobile: Detection'),
    );
    //Radio Options. Shows a list of devices to ignore detection.
    $devices = $this->devices();
    if (module_exists('wurfl')) {
      $form = $this->devices($form, $context, 'form');
    }
    //If we have empty devices, then something has been set wrong. Indicate so.
    if (empty($devices)) {
      $MSG = 'There are no devices to detect, please make sure that you have updated WURFL and made a test.';
      $form['detect_mobile'] = array(
        '#value' => t($MSG),
        '#attributes' => array('class' => 'context-condition-detect-mobile-options-form-error'),
      );
    }
    unset($devices);
    return $form;
  }

  /**
   * Override of execute().
   */
  function execute() {
    if ($this->condition_used()) {
       foreach ($this->get_contexts() as $context) {
        //Get the options, and then check our device.
        $options = $this->fetch_from_context($context, 'options');
        //Detect if we have a mobile device or not.
        $success = $this->detect($options['detect_mobile']);
        if ($success === TRUE) {
          $this->condition_met($context);
        }
      }
    }
  }

  /**
   * @name devices($form = array(), $context = NULL, $op = NULL)
   * @param <type> $form
   * @param <type> $context
   * @param <type> $op
   * @return <type>
   */
  protected function devices($form = array(), $context = NULL, $op = NULL) {
    $options = $this->fetch_from_context($context, 'options');
    //Set our query.
    $query  = db_query("SELECT did, id, brand_name, model_name, fall_back, capabilities FROM {wurfl_devices}
      WHERE model_name != ''
      GROUP BY model_name
      ORDER BY id, model_name ASC
    ");
    //Lopo through devices, and determine if it should be allowed in this list. If it is a wireless device, has HTML support or is a touch screen or is a tablet then we add it into this list.
    while ($device = db_fetch_object($query)) {
      //Get the capabilities and unserialize.
      $capabilities = unserialize($device->capabilities);
      //If we are a tablet, or wireless device, or device has a touchscreen pointing method, or the device claims it can support the web, let in the exclusion list.
      if ($capabilities['product_info']['is_tablet'] || $capabilities['product_info']['is_wireless_device'] || $capabilities['product_info']['pointing_method'] == 'touchscreen' || $capabilities['product_info']['device_claims_web_support']) {
        //Handle NULL brand names.
        $device->brand_name = (empty($device->brand_name)) ? 'Unknown' : $device->brand_name;
        //Set our label to include the model name and the fall back for this device without a version id attached.
        $device->id = preg_replace('/_ver[0-9a-zA-z]+/i', '', $device->id);
        $label['@model']     = $device->model_name;
        $label['!fall_back'] = '<div class="device-id"><small>id: ' .$device->id .'</small></div>';
        //Set our devices as an array piece.
        $devices[$device->brand_name][$device->id] = t('@model !fall_back', $label);
        //Set a form variable, for our devices.
        if ($op == 'form') {
          //Get the default values.
          $default_values = !empty($options['detect_mobile']) ? $options['detect_mobile'][$device->brand_name] : array_keys($devices[$device->brand_name]);
          $form['detect_mobile'][$device->brand_name] = array(
            '#title'         => t($device->brand_name),
            '#description'   => t('To ignore a device, check the box next to it. Ignored devices are in orange, and will not trigger this condition.'),
            '#type'          => 'checkboxes',
            '#options'       => $devices[$device->brand_name],
            '#default_value' => $default_values,
            '#attributes'    => array(
                'class' => 'context-condition-detect-mobile-options-form-devices',
            ),
            '#suffix'        => '<hr />',
          );
        }
      }
    }
    if (!empty($form['detect_mobile']) && is_array($form['detect_mobile'])) {
      ksort($form['detect_mobile']);
    }
    //Switch $op, if we need a form fromt he devices list, or just the devices array.
    switch ($op) {
      case 'form':
        //Return the form if we need a form.
        return $form;
        break;
      default:
        return $devices;
    }
  }

  /**
   *
   * @global <type> $wurfl_device
   * @param <type> $options
   * @return <type>
   */
  protected function detect($options = NULL) {
    //If our script name does not fall under index.php, then ALWAYS return FALSE
    //This will keep maintenance procedures working properly, when called outside of the normal drupal root.
    if (!strstr($_SERVER['SCRIPT_NAME'], 'index.php')) {
      return FALSE;
    }
    //User Agents to ignore. @todo - create a way to make this an admin interface.
    $this->ignoreAgents = array(
      'ipad',
    );
    //Operating System User Agents to match. @todo - create a way to make this an admin interface.
    $this->os = array(
      'Windows',
      'Macintosh',
      'Linux',
      'FreeBSD',
      'Solaris',
    );
    //Crawlers. @todo - create a way to make this an admin interface.
    $this->crawlers = array(
      'bot',
      'slurp',
      'spider',
      'crawl',
      'sniff',
      'altavista',
      'scooter',
      'tarantula',
      'teoma',
      'fast',
      'mediapartners-google',
      'nutch',
      'yahoofeedseeker',
      'googleimages',
      'yandeximages',
      'libwww-perl',
      'simpy',
      'aspsearch',
      'scoutjet',
    );
    $ignore = array_merge($this->os, $this->crawlers);
    //Turn the ignore array into a piped preg_match query.
    $ignore = implode('|', $ignore);
    //If we match anything on the ignore list, and do not have mobile in the user agent string, return false.
    if (preg_match('/('.$ignore.')/i', $_SERVER['HTTP_USER_AGENT'], $matches) && !strstr(strtoupper($_SERVER['HTTP_USER_AGENT']), 'MOBILE') || empty($_SERVER['HTTP_USER_AGENT'])) {
      //Return Not a mobile device.
      return FALSE;
    } else {
      global $wurfl_device;
      //Handle NULL brand names.
      $wurfl_device->capabilities['brand_name'] = (empty($wurfl_device->capabilities['brand_name'])) ? 'Unknown' : $wurfl_device->capabilities['brand_name'];
      //Sanitize fall back.
      $fallback = $wurfl_device->capabilities['fall_back'];
      $fallback = preg_replace('/_ver[0-9a-zA-z]+/i', '', $fallback);
      //If the value is zero (0) then we are ignoring this item.
      if (isset($options[$wurfl_device->capabilities['brand_name']][$fallback]) && $options[$wurfl_device->capabilities['brand_name']][$fallback] == 0) {
        //Returned Mobile device, ignored.
        return 'IGNORED';
      } else {
        //Return is a mobile device.
        return TRUE;
      }
    }
  }
}
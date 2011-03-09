<?php
/**
 * Expose redirection to a mobile URL as a reaction.
 */
class context_reaction_redirect_to_mobile extends context_reaction {

  /**
   * Allows Administrators to set a redirect URL for a mobile site.
   */
  function options_form($context) {
    //Saved Values.
    $values = $this->fetch_from_context($context);
    //Form.
    $form = array(
      '#tree'    => TRUE,
      '#title'   => t('Redirect To Mobile URL'),
      'redirect_to_mobile' => array(
        '#title'            => t('Redirection URL'),
        '#description'      => t('Set a valid url, this reaction will redirect you there.'),
        '#type'             => 'textfield',
        '#default_value'    => isset($values['redirect_to_mobile']) ? $values['redirect_to_mobile'] : 'mobile',
        '#required'         => TRUE,
        '#element_validate' => array('_redirect_to_mobile_validate'),
      ),
      'preserve_original_destination' => array(
        '#type' => 'radios',
        '#title' => t('Preserve Destination'),
        '#description' => t('If a mobile device lands on a NON-mobile page, and there IS a mobile page, then redirect to that page. OR, if the mobile device lands on a NON-mobile Page, FORCE the mobile page to always redirect to the above BASE URL.'),
        '#options'     => array(
          0 => t('FORCE Redirect To Mobile Base URL only'),
          1 => t('Redirect to the destination URL'),
        ),
        '#default_value' => isset($values['preserve_original_destination']) ? $values['preserve_original_destination'] : 0,
      ),
    );
    return $form;
  }

  /**
   * Set the active theme.
   * @todo - make this so that it preserves the destination to which you are going to and find a mobile url alias for that particular destination.
   * If that destination exists, then we push them over to the mobile destination, and then redirect them there.
   */
  function execute() {
    foreach (context_active_contexts() as $context) {
      if (!empty($context->reactions[$this->plugin]['redirect_to_mobile']) && strstr($_SERVER['SCRIPT_NAME'], 'index.php')) {
        //Saved Values.
        $values = $this->fetch_from_context($context);
        //If we are using a node, then the path will not be an alias, so we need to find the mobile alias for that path.
        $values['redirect_to_mobile'] = drupal_get_path_alias($values['redirect_to_mobile']);
        //We need to get the destination to which you are going to, using q= because this is your entry point to Drupal.
        //We then do a quick check to see if the $values['redirect_to_mobile'] uri has a Drupal equivilent, if it does, go to that url, if not - go to the base url.
        $destination = url($values['redirect_to_mobile'] . url($_GET['q']));
        if (drupal_lookup_path('alias', $destination) && $values['preserve_original_destination'] != 0) {
          //If we have a valid path, go to that path.
          drupal_goto($destination);
        } else {
          //If we do not have a valid path, default to $values['redirect_to_mobile'] because we KNOW that is a valid system path.
          drupal_goto($values['redirect_to_mobile']);
        }
      }
    }
  }
}
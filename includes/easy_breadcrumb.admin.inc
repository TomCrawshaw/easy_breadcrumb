<?php

/**
 * @file
 * Module's admin forms.
 */

/**
 * The default settings page.
 *
 * @param String $op
 *   operation.
 *
 * @return String
 *   the rendered form.
 */
function _easy_breadcrumb_settings_page($op = NULL) {
  return drupal_get_form('_easy_breadcrumb_general_settings_form');
}

/**
 * General configuration form.
 *
 * @return Assoc
 *   renderable form.
 */
function _easy_breadcrumb_general_settings_form() {

  // Fieldset for grouping general settings fields.
  $fieldset_general = array(
    '#type' => 'fieldset',
    '#title' => t('General settings'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  $fieldset_general[EasyBreadcrumbConstants::DISABLE_DEFAULT_DRUPAL_BREADCRUMB] = array(
    '#type' => 'checkbox',
    '#title' => t("Disable the default Drupal's breadcrumb"),
    '#default_value' => variable_get(EasyBreadcrumbConstants::DISABLE_DEFAULT_DRUPAL_BREADCRUMB, TRUE),
    '#description' => t("¿Should this module disable the default Drupal's breadcrumb?"),
  );

  $fieldset_general[EasyBreadcrumbConstants::INCLUDE_HOME_SEGMENT] = array(
    '#type' => 'checkbox',
    '#title' => t("Include the front page as the first segment in the breadcrumb"),
    '#default_value' => variable_get(EasyBreadcrumbConstants::INCLUDE_HOME_SEGMENT, TRUE),
    '#description' => t("¿Should the front page be included as a segment in the breacrumb?"),
  );

  $fieldset_general[EasyBreadcrumbConstants::INCLUDE_TITLE_SEGMENT] = array(
    '#type' => 'checkbox',
    '#title' => t("Include the current page's title as a segment in the breadcrumb"),
    '#default_value' => variable_get(EasyBreadcrumbConstants::INCLUDE_TITLE_SEGMENT, TRUE),
    '#description' => t("¿Should the current page's title be included as a segment in the breacrumb?"),
  );

  $fieldset_general[EasyBreadcrumbConstants::TITLE_FROM_PAGE_WHEN_AVAILABLE] = array(
    '#type' => 'checkbox',
    '#title' => t("Use the page's title when available"),
    '#default_value' => variable_get(EasyBreadcrumbConstants::TITLE_FROM_PAGE_WHEN_AVAILABLE, TRUE),
    '#description' => t("¿Use the page's title when it is available instead of always trying to
      deduce it from the URL?"),
  );

  $fieldset_general[EasyBreadcrumbConstants::TITLE_SEGMENT_AS_LINK] = array(
    '#type' => 'checkbox',
    '#title' => t("Make the page's title segment a link"),
    '#default_value' => variable_get(EasyBreadcrumbConstants::TITLE_SEGMENT_AS_LINK, FALSE),
    '#description' => t("¿Prints the page's title segment as a link?"),
  );

  $fieldset_general[EasyBreadcrumbConstants::SEGMENTS_SEPARATOR] = array(
    '#type' => 'textfield',
    '#title' => t('Segments separator'),
    '#description' => t("Separator to be used between the breadcrumb's segments."),
    '#default_value' => variable_get(EasyBreadcrumbConstants::SEGMENTS_SEPARATOR, '>>'),
  );

  $fieldset_general[EasyBreadcrumbConstants::CAPITALIZATOR_MODE] = array(
    '#type' => 'select',
    '#title' => t("Segments title's transformation mode"),
    '#options' => array(
      'none' => t('None'),
      'ucwords' => t("Capitalize the first letter of each word in the segment"),
      'ucfirst' => t("Only capitalize the first letter of the whole segment"),
    ),
    '#description' => t("Choose the transformation mode you want to apply to the
      segments (E.g.: 'blog/once-a-time' -> 'Home >> Blog >> Once a Time')."),
    '#default_value' => variable_get(EasyBreadcrumbConstants::CAPITALIZATOR_MODE, 'ucwords'),
  );

  // Formats the ignored-words array as space separated list of words
  // (word1 word2 wordN) before display them.
  $capitalizator_ignored_words_arr = variable_get(EasyBreadcrumbConstants::CAPITALIZATOR_IGNORED_WORDS,
    EasyBreadcrumbConstants::$DEFAULT_IGNORED_WORDS);
  $capitalizator_ignored_words = @join(' ', $capitalizator_ignored_words_arr);

  $fieldset_general[EasyBreadcrumbConstants::CAPITALIZATOR_IGNORED_WORDS] = array(
    '#type' => 'textarea',
    '#title' => t("Words to be ignored by the 'capitalizator'"),
    '#description' => t("Enter a space separated list of words to be ignored by the 'capitalizator'. E.g.: of and.<br/>
      Note: this will be applyed only to words not at the beginning of each segment."),
    '#default_value' => $capitalizator_ignored_words,
  );

  $form = array();

  // Inserts the fieldset for grouping general settings fields.
  $form[EasyBreadcrumbConstants::MODULE_NAME] = $fieldset_general;

  // Adds buttons for processing the form.
  $form['buttons'] = array(
    'submit' => array(
      '#type' => 'submit',
      '#value' => t('Save configuration'),
    ),
    'reset' => array(
      '#type' => 'reset',
      '#value' => t('Reset to defaults'),
    ),
  );

  // Specifies the callback function for processing the form submission.
  $form['#submit'] = array('_easy_breadcrumb_admin_submit');

  // Specifies the theme for the form.
  $form['#theme'] = 'system_settings_form';

  return $form;
}

/**
 * Process the submitting of the administration form of this module.
 *
 * @param Assoc $form
 *   renderable form.
 * @param Assoc $form_state
 *   form state.
 */
function _easy_breadcrumb_admin_submit($form, &$form_state) {

  $values = $form_state['values'];

  $disable_default_drupal_breadcrumb = $values[EasyBreadcrumbConstants::DISABLE_DEFAULT_DRUPAL_BREADCRUMB];
  $include_home_segment = $values[EasyBreadcrumbConstants::INCLUDE_HOME_SEGMENT];
  $include_title_segment = $values[EasyBreadcrumbConstants::INCLUDE_TITLE_SEGMENT];
  $title_from_page_when_available = $values[EasyBreadcrumbConstants::TITLE_FROM_PAGE_WHEN_AVAILABLE];
  $title_segment_as_link = $values[EasyBreadcrumbConstants::TITLE_SEGMENT_AS_LINK];
  $segment_separator = $values[EasyBreadcrumbConstants::SEGMENTS_SEPARATOR];
  $capitalizator_mode = $values[EasyBreadcrumbConstants::CAPITALIZATOR_MODE];
  // Stores the list of ignored words as an array. E.g.: array('of','and').
  $capitalize_ignored_words = $values[EasyBreadcrumbConstants::CAPITALIZATOR_IGNORED_WORDS];
  $capitalizator_ignored_words_arr = array();
  $capitalize_ignored_words_arr_aux = explode(' ', $capitalize_ignored_words);
  foreach ($capitalize_ignored_words_arr_aux as $k => $v) {
    $v = trim($v);
    if ($v !== '') {
      $capitalizator_ignored_words_arr[$v] = $v;
    }
  }

  variable_set(EasyBreadcrumbConstants::DISABLE_DEFAULT_DRUPAL_BREADCRUMB, $disable_default_drupal_breadcrumb);
  variable_set(EasyBreadcrumbConstants::SEGMENTS_SEPARATOR, $segment_separator);
  variable_set(EasyBreadcrumbConstants::INCLUDE_HOME_SEGMENT, $include_home_segment);
  variable_set(EasyBreadcrumbConstants::INCLUDE_TITLE_SEGMENT, $include_title_segment);
  variable_set(EasyBreadcrumbConstants::TITLE_FROM_PAGE_WHEN_AVAILABLE, $title_from_page_when_available);
  variable_set(EasyBreadcrumbConstants::TITLE_SEGMENT_AS_LINK, $title_segment_as_link);
  variable_set(EasyBreadcrumbConstants::CAPITALIZATOR_MODE, $capitalizator_mode);
  variable_set(EasyBreadcrumbConstants::CAPITALIZATOR_IGNORED_WORDS, $capitalizator_ignored_words_arr);

  $success_msg = t("The module's settings have been saved.");
  $success_msg = check_plain($success_msg);
  drupal_set_message($success_msg);
}
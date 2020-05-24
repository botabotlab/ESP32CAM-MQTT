void camera_init() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  config.fb_count = 1;
  config.jpeg_quality = 10;
  config.frame_size = FRAMESIZE_VGA;

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
}

void take_picture() {
  if (CONFIG["flash"] == 1) {
    digitalWrite(4, HIGH);
  }
  camera_fb_t * fb = NULL;
  fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    return;
  }
  if (MQTT_MAX_PACKET_SIZE == 128) {
    //SLOW MODE (increase MQTT_MAX_PACKET_SIZE)
    client.publish_P(topic_UP, fb->buf, fb->len, false);
  }
  else {
    //FAST MODE (increase MQTT_MAX_PACKET_SIZE)
    client.publish(topic_UP, fb->buf, fb->len, false);
  }
  Serial.println("CLIC");
  esp_camera_fb_return(fb);

  digitalWrite(4, LOW);
}

void load_config() {
  File config_file = SPIFFS.open("/config.json", "r");
  if (!config_file) {
    Serial.println("Failed to open file for reading");
    return;
  }
  //String  config_string = config_file.readString();
  deserializeJson(CONFIG, config_file.readString());
  config_file.close();

  sensor_t * s = esp_camera_sensor_get();
  s->set_vflip(s, CONFIG["vflip"]); //0 - 1
  s->set_hmirror(s, CONFIG["hmirror"]); //0 - 1

  s->set_colorbar(s, CONFIG["colorbar"]); //0 - 1
  s->set_special_effect(s, CONFIG["special_effect"]); //0 -

  s->set_quality(s, CONFIG["quality"]); // 0 - 63

  s->set_gainceiling(s, (gainceiling_t)(int)CONFIG["gain"]); // 0 - 6
  s->set_brightness(s, CONFIG["brightness"]); // -2 - 2
  s->set_contrast(s, CONFIG["contrast"]); // -2 - 2
  s->set_saturation(s, CONFIG["saturation"]); // -2 - 2

  s->set_awb_gain(s, CONFIG["awb_gain"]); // 0 - 1
  s->set_wb_mode(s, CONFIG["wb_mode"]); // 0 - 4

  s->set_framesize(s, (framesize_t)(int)CONFIG["resolution"]);// QVGA|CIF|VGA|SVGA|XGA|SXGA|UXGA
}

void edit_config() {
  sensor_t * s = esp_camera_sensor_get();
  if (CONFIGTEMP.containsKey("vflip")) {
    CONFIG["vflip"] = CONFIGTEMP["vflip"];
  }
  if (CONFIGTEMP.containsKey("hmirror")) {
    CONFIG["hmirror"] = CONFIGTEMP["hmirror"];
  }
  if (CONFIGTEMP.containsKey("colorbar")) {
    CONFIG["colorbar"] = CONFIGTEMP["colorbar"];
  }
  if (CONFIGTEMP.containsKey("special_effect")) {
    CONFIG["special_effect"] = CONFIGTEMP["special_effect"];
  }
  if (CONFIGTEMP.containsKey("quality")) {
    CONFIG["quality"] = CONFIGTEMP["quality"];
  }
  if (CONFIGTEMP.containsKey("gain")) {
    CONFIG["gain"] = CONFIGTEMP["gain"];
  }
  if (CONFIGTEMP.containsKey("brightness")) {
    CONFIG["brightness"] = CONFIGTEMP["brightness"];
  }
  if (CONFIGTEMP.containsKey("contrast")) {
    CONFIG["contrast"] = CONFIGTEMP["contrast"];
  }
  if (CONFIGTEMP.containsKey("saturation")) {
    CONFIG["saturation"] = CONFIGTEMP["saturation"];
  }
  if (CONFIGTEMP.containsKey("awb_gain")) {
    CONFIG["awb_gain"] = CONFIGTEMP["awb_gain"];
  }
  if (CONFIGTEMP.containsKey("wb_mode")) {
    CONFIG["wb_mode"] = CONFIGTEMP["wb_mode"];
  }
  if (CONFIGTEMP.containsKey("resolution")) {
    CONFIG["resolution"] = CONFIGTEMP["resolution"];
  }
  if (CONFIGTEMP.containsKey("flash")) {
    CONFIG["flash"] = CONFIGTEMP["flash"];
  }
  save_config();
}
void save_config() {
  //Open the config.json file (Write Mode)
  String config_string;
  File config_file = SPIFFS.open("/config.json", "w");
  if (!config_file) {
    Serial.println("Failed to open file (Writing mode)");
    return;
  }
  //Save and close the JSON file
  serializeJson(CONFIG, config_string);
  if (config_file.println(config_string)) {
    Serial.println("New Config");
    Serial.println(config_string);
  } else {
    Serial.println("File write failed");
  }
  config_file.close();
  load_config();
}

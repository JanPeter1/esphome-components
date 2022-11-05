
/**
 * @brief Simple motion detection sensor that uses the WiFi signal strength
 *        signal (RSSI) to detect motions.
 *
 * @author Jan Peter Riegel <JanPeter1@familie-riegel.de>
 * Copyright (c) 2022
 */

#include "wifi_csi.h"

static const char *const TAG = "wifi_csi";
extern esphome::wifi::WiFiComponent *esphome::wifi::global_wifi_component;  // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

esphome::wifi_csi::CsiSensor::CsiSensor()
: PollingComponent()
, binary_sensor::BinarySensor()
, m_pollingInterval(100)
, m_bufferSize(100)
, m_sensitivity(2)
, m_rssi(nullptr)
{
    set_update_interval(m_pollingInterval);
    this->set_device_class("motion");
}

esphome::wifi_csi::CsiSensor::~CsiSensor()
{
    if (m_rssi) {
        free(m_rssi);
        m_rssi = nullptr;
    }
}

float esphome::wifi_csi::CsiSensor::get_setup_priority() const
{
    return esphome::setup_priority::AFTER_WIFI;
}

void esphome::wifi_csi::CsiSensor::dump_config()
{
    ESP_LOGCONFIG(TAG, "Wifi CSI:");
    ESP_LOGCONFIG(TAG, "polling interval: %dms", m_pollingInterval);
    ESP_LOGCONFIG(TAG, "buffer size: %d", m_bufferSize);
    ESP_LOGCONFIG(TAG, "sensitivity: %d", m_sensitivity);
}

void esphome::wifi_csi::CsiSensor::set_timing(int pollingInterval)
{
    m_pollingInterval = pollingInterval;
    set_update_interval(pollingInterval);
}

void esphome::wifi_csi::CsiSensor::set_sensitivity(int sensitivity)
{
    m_sensitivity = sensitivity;
}

void esphome::wifi_csi::CsiSensor::set_buffer_size(int bufferSize)
{
    m_bufferSize = bufferSize;
    if (m_rssi) free(m_rssi);
    m_rssi = reinterpret_cast<int*>(malloc(m_bufferSize * sizeof(int)));
}

void esphome::wifi_csi::CsiSensor::update() {
    static int idx = 0;   // pointer inside rssi
    static int cnt = 0;   // number of values inside rssi
    static int sum = 0;   // sum of all rssi values

    if (m_rssi) {            
        int currentRssi = 0;
        if (nullptr != esphome::wifi::global_wifi_component) currentRssi = esphome::wifi::global_wifi_component->wifi_rssi();
        if (cnt == m_bufferSize) {
            sum -= m_rssi[idx];  // we will overwrite the oldest value, so remove it from the current sum
        } else {
            cnt += 1;
        }
        m_rssi[idx] = currentRssi;
        idx = (idx + 1) % m_bufferSize;
        sum += currentRssi;

        int avgerageRssi = sum / cnt;
        int dev = abs(currentRssi - avgerageRssi);
        bool motion = (dev >= m_sensitivity);
        publish_state(motion);

        // log every 101th interval
        static int lcnt = 0;
        lcnt++;
        if (lcnt % 101 == 0) ESP_LOGD(TAG, "idx: %d, cnt: %d: avg: %d, current: %d, sensitvity: %d, motion: %d", idx, cnt, avgerageRssi, currentRssi, m_sensitivity, motion);
    } else {
        set_buffer_size(m_bufferSize);  // create the rssi buffer
    }
}

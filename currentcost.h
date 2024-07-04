#include <string>
#include "esphome.h"
#include "rapidxml/rapidxml.hpp"

static const char *TAG = "currentcost";

class CurrentCostSensor : public Component, public UARTDevice {
 public:
  Sensor *ch1 = new Sensor();
  Sensor *ch2 = new Sensor();
  Sensor *ch3 = new Sensor();

  CurrentCostSensor(UARTComponent *parent) : UARTDevice(parent) {}

  void setup() override {
    // nothing to do here
  }

  int readline(int readch, char *buffer, int len)
  {
    static int pos = 0;
    int rpos;

    if (readch > 0) {
      switch (readch) {
        case '\r': // Return on CR
        case '\n': // Return on LF
          rpos = pos;
          pos = 0;  // Reset position index ready for next time
          return rpos;
        default:
          if (pos < len-1) {
            buffer[pos++] = readch;
            buffer[pos] = 0;
          }
      }
    }
    // No end of line has been found, so return -pos to show how far we have got.
    return -pos;
  }

  boolean expect_node(rapidxml::xml_node<> *node, std::string name) {
    if (node && node->name() != name) {
      ESP_LOGD(TAG, "Unexpected node: '%s', expected '%s'", node->name(), name.c_str());
      return false;
    }
    return true;
  }

  void publish(char *buffer) {
	rapidxml::xml_document<> *xml_doc = new rapidxml::xml_document<>();
    try {
      xml_doc->parse<0> (buffer);
      rapidxml::xml_node<> *node, *watts;

      node = xml_doc->first_node();
      if (!expect_node(node, "msg")) return;

      node = node->first_node();
      if (!expect_node(node, "src")) return;

      node = node->next_sibling();
      if (!expect_node(node, "dsb")) return;

      node = node->next_sibling();
      if (!expect_node(node, "time")) return;

      node = node->next_sibling();
      if (!expect_node(node, "tmpr")) return;

      node = node->next_sibling();
      if (!expect_node(node, "sensor")) return;

      node = node->next_sibling();
      if (!expect_node(node, "id")) return;

      node = node->next_sibling();
      if (!expect_node(node, "type")) return;

      node = node->next_sibling();
      if (!expect_node(node, "ch1")) return;
      watts = node->first_node();
      if (!expect_node(watts, "watts")) return;
      ch1->publish_state(atof(watts->value()));

      node = node->next_sibling();
      if (!expect_node(node, "ch2")) return;
      watts = node->first_node();
      if (!expect_node(watts, "watts")) return;
      ch2->publish_state(atof(watts->value()));

      node = node->next_sibling();
      if (!expect_node(node, "ch3")) return;
      watts = node->first_node();
      if (!expect_node(watts, "watts")) return;
      ch3->publish_state(atof(watts->value()));

    } catch (rapidxml::parse_error& e) {
        ESP_LOGD(TAG, "Parse error: %s", e.what());
    } catch (std::exception& e) {
        ESP_LOGD(TAG, "Caught exception: %s", e.what());
    }
	delete xml_doc;
  }

  void loop() override {
    const int max_line_length = 1024;
    static char buffer[max_line_length];
	int a;
	unsigned long start = millis();
    while ((a = available())) {
	  uint8_t read_buf[a];
	  if (read_array(read_buf, a)) {
	    int i;
	    for (i=0; i<a; i++) {
          int got = readline(read_buf[i], buffer, max_line_length);
          if (got > 0 && got < max_line_length - 1) {
		    unsigned long end = millis();
            ESP_LOGD(TAG, "Took %lu to read: '%s'", (end - start), buffer);
			start = millis();
			publish(buffer);
		    end = millis();
            ESP_LOGV(TAG, "Took %lu to publish", (end - start));
          }
		}
	  }
    }
  }
};


#pragma once
#ifndef ARDUINO_ARTNET_H
#define ARDUINO_ARTNET_H

// Spec (Art-Net 4) : http://artisticlicence.com/WebSiteMaster/User%20Guides/art-net.pdf
// Packet Summary : https://art-net.org.uk/structure/packet-summary-2/
// Packet Definition : https://art-net.org.uk/structure/streaming-packets/artdmx-packet-definition/

// using Artnet = arx::artnet::Manager;
// using ArtnetSender = arx::artnet::Sender;
using ArtnetReceiver = arx::artnet::Receiver;

#include "ArtnetCommon.h"

#endif  // ARDUINO_ARTNET_H

#pragma once
#ifndef ARTNET_ETHER_H
#define ARTNET_ETHER_H

#define ARTNET_ENABLE_ETHER

#include "Artnet/ArtnetCommon.h"
#include "Artnet/util/ArxContainer/ArxContainer.h"
#include "Artnet/util/ArxTypeTraits/ArxTypeTraits.h"
#include "Artnet/util/TeensyDirtySTLErrorSolution/TeensyDirtySTLErrorSolution.h"
#include <Arduino.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

using Artnet = arx::artnet::Manager<EthernetUDP>;
using ArtnetSender = arx::artnet::Sender<EthernetUDP>;
using ArtnetReceiver = arx::artnet::Receiver<EthernetUDP>;

#endif // ARTNET_ETHER_H

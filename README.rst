VFRMap
======

This is the source code for my `live aviation weather map <https://aeracode.org/2020/07/10/aviation-weather-map/>`_. It's designed for the Particle Photon, and would be easy to adapt to any Particle board, and slightly more difficult to adapt to a normal Arduino-compatible board (you'd need to handle the server SSL connection and JSON decoding yourself).

It uses the Adafruit ``neopixel`` library (version 1.0) to drive the LEDs from a single digital output pin; the LEDs' power and ground are hooked up to the microprocessor's power regulator pins.

To drive this particular piece of code using Particle, you need a Webhook integration tied into the ``get_metar`` event with the following settings:

* Full URL: ``https://avwx.rest/api/metar/{{{PARTICLE_EVENT_VALUE}}}?format=json&onfail=cache``
* Request type: GET
* Request format: Query Parameters
* Headers: ``{"Authorization": "your-avwx-token-here"}``
* Response topic: ``metar_response``
* Response template: ``{{{station}}},{{{flight_rules}}}``
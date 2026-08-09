@@
     COMMAND_SERIAL.begin(9600);
+    // Disable marcduinoSerial reading from the hardware serial since we
+    // now use a dedicated poll+queue mechanism to avoid lost/fragmented
+    // bytes when logging or processing is blocking.
+    marcduinoSerial.setStream((Stream*)nullptr);
@@
     // Fast, non-blocking serial receive and queuing
     pollSerial1AndQueue();
@@
     // Regular processing
     AnimatedEvent::process();
     
     DomeButton();

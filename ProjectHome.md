SimpleModbus is a collection of Arduino libraries that enables you to communicate serially using the Modicon Modbus RTU protocol. Both SimpleModbusMaster & SimpleModbusSlave implements function 3 and 16 in addition SimpleModbusMaster implements function 1, 2, 4 and 15. Broadcasting is supported on both libraries for function 16 and 15 on the master. Both libraries share the exact same API. There are only two required functions modbus\_update() and modbus\_configure(). Both libraries have been tested extensively on commercial equipment like LS Industrial PLC's, Schneider equipment, Ascon equipment, Siemens PLC's, Selco alarm panels and various commercial panel meters and indicators. In addition to this SimpleModbusSlave has also been tested on LS InfoU SCADA system, Mango M2M SCADA, FreeSCADA 2, Modbus Poll, the free QModbus slave tester and the very handy modbus slave tester,<a href='http://www.plcs.net/downloads/index.php?&direction=0&order=&directory=Modicon'> Mtester</a>

The libraries are very robust and non blocking, all communication takes place in the background allowing your code to run without any delay in execution (well almost). The examples and libraries are well commented and should get you up and running quickly.

The libraries are written with an object orientated approach using C and does not make use of C++ object constructs. This makes it easier to port to other platforms which only supports a C compiler.

The libraries were also tested on a PIC18F26K80 microcontroller using the Lupic18 C compiler but should work on any microcontroller and compiler provided that you support the
library with your own hardware serial library. Specifically the following functions:

available() - To check if something is in the receive buffer<br>
read() -  To extract the bytes from the buffer<br>
write() - To transmit raw bytes out<br>
begin() - To initialize the serial routine<br>

Note!!<br>
<br>
The implementations does not fully comply with the Modbus over serial line specification. Specifically the frame and inter character timing does not exactly adhere to the modbus standard but a simple method is implemented that combines both these delays and works very well indeed.<br>
<br>
The frame and character timing is probably the most important aspect of modbus since this defines the start and end of a frame and inter character timing. In order to do this correctly you will have to use a timer interrupt set at the inter character transmission time and most likely you must implement your own serial library (or parts there of) since the timer interrupt must be cleared by the reception flag of the serial receive hardware character buffer. This is quite intensive and in my mind veers away from the KISS principal.<br>
<br>
In my line of work in Marine and Industrial electronics engineering I have tested the libraries extensively with marine and industrial equipment I come across on a usual day to day basis. I did not once not manage to connect to a master or slave due to the frame and inter character timing limitations.<br>
<br>
The libraries have been updated to support Arduino V1.05 and later versions.<br>
<br>
Please check the "Issues" page for a description on updates and various other helpful information.<br>
<br>
Check the arduino forum <a href='http://forum.arduino.cc/index.php?topic=176142.0'>thread</a> for allot of useful information.<br>
<br>
Google has stopped the upload of new files. The latest libraries can be obtained <a href='https://drive.google.com/folderview?id=0B0B286tJkafVSENVcU1RQVBfSzg&usp=drive_web'>here</a>

Feedback is welcomed so I can make the libraries even more robust. Mail me at bester.juan@gmail.com
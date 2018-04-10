# Exhaustive-evaluation-of-TCP-Selective-Acknowledgments-in-WiFi-environments
This project aims to evaluate the performance of different TCP extensions in wireless networks, with and without SACK.
## Overview of TCP-SACK
### What is TCP-SACK? <br/>
Selective Acknowledgements are a refinement of TCP's traditional "cumulative" acknowledgements. 

TCP uses a cumulative acknowledgment scheme in which received segments that are not at the left edge of the receiver window are not acknowledged. This forces the sender to either wait a roundtrip time to find out about each lost packet, or to unnecessarily retransmit segments which have been correctly received.  <br/>

With selective acknowledgments, the data receiver can inform the sender about all segments that have arrived successfully, so the sender need retransmit only the segments that have actually been lost.<br/>
The example of TCP-SACK is  [Click Here](https://github.com/palarnab23/Exhaustive-evaluation-of-TCP-Selective-Acknowledgments-in-WiFi-environments/wiki/Example-of-TCP-SACK)

### References
1. TCP Selective Acknowledgment Options   RFC 2018  https://tools.ietf.org/html/rfc2018 

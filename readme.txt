Sample Fhem Configuration:
// DEVICE
define HBW_LC_RGB1_CV_HBW4101994 HM485 56ABD5EA
attr HBW_LC_RGB1_CV_HBW4101994 firmwareVersion 3.06
attr HBW_LC_RGB1_CV_HBW4101994 model HBW_LC_RGB1_CV
attr HBW_LC_RGB1_CV_HBW4101994 room HM485
attr HBW_LC_RGB1_CV_HBW4101994 serialNr HBW4101994
// LOG
define FileLog_HBW_LC_RGB1_CV_HBW4101994 FileLog ./log/HBW_LC_RGB1_CV_HBW4101994-%Y.log HBW_LC_RGB1_CV_HBW4101994
attr FileLog_HBW_LC_RGB1_CV_HBW4101994 logtype text
attr FileLog_HBW_LC_RGB1_CV_HBW4101994 room HM485
// HUE Channel
define HBW_LC_RGB1_CV_HBW4101994_01 HM485 56ABD5EA_01
attr HBW_LC_RGB1_CV_HBW4101994_01 eventMap /level 0:rot/level 33:gruen/level 66:blau/
attr HBW_LC_RGB1_CV_HBW4101994_01 firmwareVersion 3.06
attr HBW_LC_RGB1_CV_HBW4101994_01 model HBW_LC_RGB1_CV
attr HBW_LC_RGB1_CV_HBW4101994_01 room HM485
attr HBW_LC_RGB1_CV_HBW4101994_01 serialNr HBW4101994
attr HBW_LC_RGB1_CV_HBW4101994_01 subType dimmer
attr HBW_LC_RGB1_CV_HBW4101994_01 webCmd level:rot:gruen:blau
// Satuation Channel
define HBW_LC_RGB1_CV_HBW4101994_02 HM485 56ABD5EA_02
attr HBW_LC_RGB1_CV_HBW4101994_02 firmwareVersion 3.06
attr HBW_LC_RGB1_CV_HBW4101994_02 model HBW_LC_RGB1_CV
attr HBW_LC_RGB1_CV_HBW4101994_02 room HM485
attr HBW_LC_RGB1_CV_HBW4101994_02 serialNr HBW4101994
attr HBW_LC_RGB1_CV_HBW4101994_02 subType dimmer
attr HBW_LC_RGB1_CV_HBW4101994_02 webCmd level
// Value Channel
define HBW_LC_RGB1_CV_HBW4101994_03 HM485 56ABD5EA_03
attr HBW_LC_RGB1_CV_HBW4101994_03 eventMap /level 100:an/level 0:aus/
attr HBW_LC_RGB1_CV_HBW4101994_03 firmwareVersion 3.06
attr HBW_LC_RGB1_CV_HBW4101994_03 model HBW_LC_RGB1_CV
attr HBW_LC_RGB1_CV_HBW4101994_03 room HM485
attr HBW_LC_RGB1_CV_HBW4101994_03 serialNr HBW4101994
attr HBW_LC_RGB1_CV_HBW4101994_03 subType dimmer
attr HBW_LC_RGB1_CV_HBW4101994_03 webCmd level:an:aus
// Fading Switch
define HBW_LC_RGB1_CV_HBW4101994_04 HM485 56ABD5EA_04
attr HBW_LC_RGB1_CV_HBW4101994_04 firmwareVersion 1.01
attr HBW_LC_RGB1_CV_HBW4101994_04 model HBW_LC_RGB1_CV
attr HBW_LC_RGB1_CV_HBW4101994_04 room HM485
attr HBW_LC_RGB1_CV_HBW4101994_04 serialNr HBW4101994
attr HBW_LC_RGB1_CV_HBW4101994_04 subType switch

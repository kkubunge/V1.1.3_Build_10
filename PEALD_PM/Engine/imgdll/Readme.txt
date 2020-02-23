+----------------+
|Revision History|
+----------------------------------------------------
  | 2000.01.01 | Initial Version
  | 2001.01.01 | Make Robot Dll
  | 2001.03.01 | Update Robot
  | 2001.06.15 | Update Robot
  | 2001.08.18 | Make FileListView Dll
  | 2004.12.14 | Update BrooksRobot
  +------------+-------------------------------------
---------------++--------+--------+--------+--------+---------+---------+--------+---------+--------+----+--+----+----+--+
FileName       || i      | i1     | i2     | i3     | i4      | i5      | d      | dp1     | dp2    |dp3 |s | s1 | s2 |s3|
---------------++--------+--------+--------+--------+---------+---------+--------+---------+--------+----+--+----+----+--+
BarGraph.dll   ||Position|Mode    |Back    |Value   |x size   |y size   |Position|LineDraw |TextDraw|    |  |    |    |  |
               ||Value   | 0:Right|  Color |  Color |         |         |Value   | 0:Not   | 0:Not  |    |  |    |    |  |
               ||for     | 1:Left |        |        |         |         |for     |   Draw  |   Draw |    |  |    |    |  |
               ||digital | 2:Up   |        |        |         |         |analog  | 1:Draw  | 1:Draw |    |  |    |    |  |
               ||type    | 3:Down |        |        |         |         |type    |         |        |    |  |    |    |  |
---------------++--------+--------+--------+--------+---------+---------+--------+---------+--------+----+--+----+----+--+
VatPos.dll     ||Position|Mode    |Size    |Value   |         |         |Position|         |        |    |  |    |    |  |
               ||Value   | 0:Use  |        |  Color |         |         |Value   |         |        |    |  |    |    |  |
               ||for     |  Analog|        |        |         |         |for     |         |        |    |  |    |    |  |
               ||digital | 1:Use  |        |        |         |         |analog  |         |        |    |  |    |    |  |
               ||type    |  Digitl|        |        |         |         |type    |         |        |    |  |    |    |  |
---------------++--------+--------+--------+--------+---------+---------+--------+---------+--------+----+--+----+----+--+
fboxtext.dll   ||Check   |x size  |y size  |Font    |Back     |TextColor|        |         |        |    |  |Text|    |  |
               ||  Status|        |        |  Size  |  Color  |if (i=i5)|        |         |        |    |  |    |    |  |
               ||        |        |        |        |         | TextColr|        |         |        |    |  |    |    |  |
               ||        |        |        |        |         |   =Black|        |         |        |    |  |    |    |  |
               ||        |        |        |        |         |else     |        |         |        |    |  |    |    |  |
               ||        |        |        |        |         | TextColr|        |         |        |    |  |    |    |  |
               ||        |        |        |        |         |   =white|        |         |        |    |  |    |    |  |
---------------++--------+--------+--------+--------+---------+---------+--------+---------+--------+----+--+----+----+--+
robot.dll      ||Wafer A |Wafer B |Wafer A |Wafer B |Finger A |Finger B |rotate  |extend   |WaferMsk|zoom|  |    |    |  |
               ||  Status|  Status|  Source|  Source|  Use    |  Use    |  value |  value  |_Mode   |    |  |    |    |  |
               ||        |        | -1:Hide| -1:Hide|   or    |   or    |        |         | (see*) |    |  |    |    |  |
               ||        |        |  Source|  Source|Wafer A  |Wafer B  |        |         |        |    |  |    |    |  |
               ||        |        |  String|  String| Result  | Result  |        |         |        |    |  |    |    |  |
---------------++--------+--------+--------+--------+---------+---------+--------+---------+--------+----+--+----+----+--+
robotBrooks.dll||Wafer A |Wafer B |Wafer A |Wafer B |Finger A |Finger B |rotate  |extend   |WaferMsk|zoom|  |    |    |  |
               ||  Status|  Status|  Source|  Source|  Use    |  Use    |  value |  value  |_Mode2  |    |  |    |    |  |
               ||        |        | -1:Hide| -1:Hide|   or    |   or    |        |         | (see*) |    |  |    |    |  |
               ||        |        |  Source|  Source|Wafer A  |Wafer B  |        |         |        |    |  |    |    |  |
               ||        |        |  String|  String| Result  | Result  |        |         |        |    |  |    |    |  |
               ||        |        |        |        |---------|---------|        |         |        |    |  |    |    |  |
               ||        |        |        |        |If = -1  |If = -1  |        |         |        |    |  |    |    |  |
               ||        |        |        |        | Wafer A | Wafer B |        |         |        |    |  |    |    |  |
               ||        |        |        |        |  Status |  Status |        |         |        |    |  |    |    |  |
               ||        |        |        |        |  Text(X)|  Text(X)|        |         |        |    |  |    |    |  |
               ||        |        |        |        |If = -2  |If = -2  |        |         |        |    |  |    |    |  |
               ||        |        |        |        | Wafer A | Wafer B |        |         |        |    |  |    |    |  |
               ||        |        |        |        |  Grid(X)|  Grid(X)|        |         |        |    |  |    |    |  |
               ||        |        |        |        |If = -3  |If = -3  |        |         |        |    |  |    |    |  |
               ||        |        |        |        | Wafer A | Wafer B |        |         |        |    |  |    |    |  |
               ||        |        |        |        |  Status |  Status |        |         |        |    |  |    |    |  |
               ||        |        |        |        |  Text(X)|  Text(X)|        |         |        |    |  |    |    |  |
               ||        |        |        |        | Wafer A | Wafer B |        |         |        |    |  |    |    |  |
               ||        |        |        |        |  Grid(X)|  Grid(X)|        |         |        |    |  |    |    |  |
---------------++--------+--------+--------+--------+---------+---------+--------+---------+--------+----+--+----+----+--+
RobotM.dll     ||Wafer   |Wafer   |WaferMsk|WaferMsk|Wafer    |Wafer    |rotate  |extend   |x move  |zoom|  |    |    |  |
               ||  Status|  Source|_Mode4  |_Mode3  | Result  | Size    |  value |  value  |  value |    |  |    |    |  |
               ||        | -1:Hide| (see*) | (see*) |         |zom+WS/10|        |         |        |    |  |    |    |  |
               ||        |  String|        |        |         |         |        |         |        |    |  |    |    |  |
---------------++--------+--------+--------+--------+---------+---------+--------+---------+--------+----+--+----+----+--+
RobotM2.dll    ||Wafer   |Wafer   |WaferMsk|WaferMsk|Wafer    |Wafer    |rotate  |extend   |y move  |zoom|  |    |    |  |
               ||  Status|  Source|_Mode4  |_Mode3  | Result  | Size    |  value |  value  |  value |    |  |    |    |  |
               ||        | -1:Hide| (see*) | (see*) |         |zom+WS/10|        |         |        |    |  |    |    |  |
               ||        |  String|        |        |         |         |        |         |        |    |  |    |    |  |
---------------++--------+--------+--------+--------+---------+---------+--------+---------+--------+----+--+----+----+--+
RobotM3.dll    ||Wafer   |Wafer   |Finger  |WaferMsk|Wafer    |Wafer    |rotate  |extend   |x move  |zoom|  |    |    |  |
RobotM4.dll    ||  Status|  Source|   Side |_Mode3  | Result  | Size    |  value |  value  |  value |    |  |    |    |  |
               ||        | -1:Hide| 0:Left | (see*) |         |zom+WS/10|        |         |        |    |  |    |    |  |
               ||        |  String| 1:Right|        |         |         |        |         |        |    |  |    |    |  |
---------------++--------+--------+--------+--------+---------+---------+--------+---------+--------+----+--+----+----+--+
FileListView   ||reload  |xstart  |ystart  |xsize   |ysize    |fontsize |        |ViewOrder|No. View|    |  |LV  |File|  |
     .dll      ||  file  |        |        |        |         |         |        |(if==0   |(if==0  |    |  |Form|Name|  |
               ||(if==1) |        |        |        |         |         |        |down,else| X,else |    |  |(see|    |  |
               ||        |        |        |        |         |         |        |up)      | O)     |    |  | *) |    |  |
---------------++--------+--------+--------+--------+---------+---------+--------+---------+--------+----+--+----+----+--+

  * WaferMsk_Mode   XXXXXXXX
                    TTTTTTTT
                    |||||||+-> 0:Wafer(Circle) , 1:Mask(Rectangle) 2:(Rec)...
                    ||||||+--> When FingerMove , Finger Move Size
                    |||||+---> 0:FingerNotMove , 1:FingerMove/Left , 2:FingerMove/Right ,
						 3:FingerMove/Left/Ext01NotMove , 4:FingerMove/Right/Ext01NotMove ,
						 5:FingerMove/Left/Ext10NotMove , 6:FingerMove/Right/Ext10NotMove ,
						 7:FingerMove/Left/Ext20NotMove , 8:FingerMove/Right/Ext20NotMove
                    ||||+----> 0:ShowUnderArm  , 1:HideUnderArm
                    |||+-----> 0:ShowFingerSide , 1:HideFingerSide
                    ||+------> 0:FingerUsewithIO(i4,i5) 1:FingerASide&Result(i4,i5) 2:FingerBSide&Result(i4,i5) 3:FingerABSide&Result(i4,i5)
                    ++-------> WaferSize(zoom+ws/10)

  * WaferMsk_Mode2    XXXXXXX
                      TTTTTTT
                      ||||||+-> 0:Wafer(Circle) , 1:Mask(Rectangle)
                      |||||+--> 0:ShowFingerSide , 1:HideFingerSide , 2:ShowWaferOnly
                      ||||+---> 0:FingerUsewithIO(i4,i5) 1:FingerASide&Result(i4,i5) 2:FingerBSide&Result(i4,i5) 3:FingerABSide&Result(i4,i5)
                      ||++----> WaferSize(zoom+ws/10)
                      |+------> FingerSide char Offset
                      +-------> 0:ShowUnderPlate             , 1:HideUnderPlate
	                        2:ShowUnderPlate+armzoomout1 , 3:HideUnderPlate+armzoomout1 ,
	                        4:ShowUnderPlate+armzoomout2 , 5:HideUnderPlate+armzoomout2 ,
	                        6:ShowUnderPlate+armzoomout3 , 7:HideUnderPlate+armzoomout3 ,
	                        8:ShowUnderPlate+armzoomout4 , 9:HideUnderPlate+armzoomout4

  * WaferMsk_Mode3        XXX
                          TTT
                          ||+-> (Rotate BodyDraw) 0:Draw , 1:Hide
                          |+--> 0:Wafer(Circle) , 1:Mask(Rectangle) 2:(Rec)...
                          +---> Arm Mode

  * WaferMsk_Mode4        XXX
                          TTT
                          ||+-> Finger_Side  0:Left , 1:Right
                          |+--> Finger_Style 0:default , 1:Style1
                          +---> Finger_Back  0:-30 , 1:-27 , 2:-24 , 3:-21 , 4:-18 , 5:-15 , 6:-12 , 7:-9 , 8:-6 , 9:0

  * LV_Form          Column1Contents:Column1Size|Column2Contents:Column2Size|Column3Contents:Column3Size|Column?Contents:Column?Size|--> upto 32

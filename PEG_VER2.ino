/*
* @Project Name : Anti-myopia appliance based on Arduino (Uno)
* @Project Code name: PEG_VER2
* @File Name : PEG_VER2.ino
* @Create Time: Apr. 4th, 2018
* @Finish Time: Apr. 7th,2018
*/

/*

iiiiiiiiiiiiiiiiiiiiiiWiiiiiiiiiiiiiiiii
iiiiiiiiiiiiiiiiiiiit;# iiiiiiiiiiiiiiii
iiiiiiiiiiiiiiiiiiiii fWiiiiiiiiiiiiiiii
iiiiiiiiiiiiiiiiiiiiiE  jiiiiiiiiiiiiiii
iiiiiii# iiiiiiiiiiii#  #iiiiitii,.iiiii
iiiiiii##itiiiiiiiiijW   E  t#######,tii
iiiiiiiK #,iiiiiii; ##   ;:   #######.ii
iiiiii;L  #iiit. ##W         LWWKE### ii
iiiiii :   #t##D             ######## ii
iiiiii     :                 ########tii
iiiiiif          ##          ###G### iii
iiiiii#          ##          #####W;iiii
iiiitt#          ##           ####.iiiii
iiiiii#                       ##W;tiiiii
iiiiii#                       #;tt:iiiii
iiiiii#                     K# it : iiii
iiiii,#                    #Gitiit:iiiii
iiiii.#                   j ii:; iiiiiii
iiiii #                   ; ii##,iiiiiii
iiiii #           #:#       iij#;iiiiiii
iiiii.#           ##.      ,i:#:iiiiiiii
iiiiif#          #         t.#.iiiiiiiii
iiiii##         #          G iiiiiiiiiii
iiiii#L    #####           Eiiiiiiiiiiii
iiiii#                     Kiiiiiiiiiiii
iiiii#                     Kiiiiiiiiiiii
iiii:#                     Wtiiiiiiiiiii
iiii #                     #iiiiiiiiiiii
iiitiW                     #iiiiiiiiiiii
iiii#:                     #iiiiiiiiiiii
iiit#                      #iiiiiiiiiiii
iii,#.                     #iiiiiiiiiiii
iii #                      #iiiiiiiiiiii
iiiL#                      #iiiiiiiiiiii
iii#E                      #iiiiiiiiiiii
				刘看山保佑！
				Bug 越来越少！
*/

#define HARDWARE_INFRARED_DISTANCER A1 // 红外测距仪 Pin 脚
#define HARDWARE_PHOTORESISTER A0 // 光敏电阻 Pin 脚
#define LED_TIPS_DARK 4 // 光线过暗时，提示LED Pin 脚
#define LED_TIPS_DISTANCE 7 // 距离过近时，提示 LED Pin 脚
#define LED_TIPS_SAFE 2 // 没有什么毛病时的 LED Pin 脚

#define HARDWARE_PHOTORESISTER_LOWLIGHT 900 //电压高于该值时判定为 『低亮度』
#define HARDWARE_BUSZZER_FREQUENCY 1000 // 蜂鸣器频率
typedef enum {
	TOO_DARK, // 过暗
	TOO_CLOSE,  // 过近
	CLOSE_AND_DARK, // 过暗、过近
	SHOW_SAFE,// 点亮安全的那盏小绿灯~[内部]
	SHOW_DANGER,//摘下绿帽子[内部]
	CLEAR_DARK, // 清除过暗效果
	CLEAR_CLOSE, // 清除过近效果
	CLEAR_ALL // 清除目前所有状态
} NOTIFYWARNING_LIST;


//************************************
// Method:   checkDark
//					 检查是否光线过暗（光敏电阻）
// FullName:  boolean checkDark() 
// Returns:   boolean (过暗(电阻值 > HARDWARE_PHOTORESISTER_LOWLIGHT)：true)
// Parameter: Null
//************************************
boolean checkDark() {
	Serial.print("[checkDark]光敏电阻两端电压值：");
	Serial.println(analogRead(HARDWARE_PHOTORESISTER));
	return  analogRead(HARDWARE_PHOTORESISTER) > HARDWARE_PHOTORESISTER_LOWLIGHT;
}


//************************************
// Method:   checkDistance
//					 检查测距仪器(GP2Y0A21YK0F)距离是否小于指定值
// FullName:  boolean checkDistance(int minDistance)
// Returns:   boolean (小于: true)
// Parameter: 
//						unsigned int minDistance			用户指定的距离
//************************************
boolean checkDistance(unsigned int minDistance) {
	double dVolt = (double) analogRead(HARDWARE_INFRARED_DISTANCER) * 5.00 / 1023.00;
	double dDistance,dDistanceRabbit;
	
	
	Serial.print("[checkDistance]此时电压：");
	Serial.println(dVolt);
	
	// 此函数存在定义域问题。
	// 当 y 值达到 3.3x (V) 的时候，delta = nan 正好是不能到达的点
	// （我也不知道为什么不能到达，但是函数解析式是这样的）
	// 因此对于 3.3x (V) 时需特判（此时距离值可以直接打表：8cm
	if (abs(dVolt - 3.38) <= 0.1) {
		return 8 < minDistance;
	}

	double delta = sqrt(-545.16*dVolt + 1781.8169);
	double denominator = 2 * dVolt + 1.226 ;
	
	dDistance = (-delta + 46) / denominator;// https://wenku.baidu.com/view/cfbd075f312b3169a451a46e.html
	dDistanceRabbit = ( delta + 46 ) / denominator;
	
	Serial.print("[checkDistance]距离值(dDistance,dDistanceRabbit)：");
	Serial.print(dDistance);
	Serial.print(",");
	Serial.println(dDistanceRabbit);

	// 因为此函数近似于二次函数，
	// 会存在两个值
	// 根据实际情况，此时应该考虑 > 5 的 x 值
	if (dDistance >  5) { 
		return dDistance < minDistance;
	} else { 
		return dDistanceRabbit < minDistance;
	}
	
}

//************************************
// Method:   notifyWarning
//					警告使用者当前环境出现问题
// FullName:  void notifyWarning(int warningType) 
// Returns:   Null
// Parameter: 
//						int warningType			警告类型 (NOTIFYWARNING_LIST)
//************************************
void notifyWarning(int warningType) {
	NOTIFYWARNING_LIST nType = (NOTIFYWARNING_LIST) warningType;
	switch (nType) {
	case TOO_DARK:
		notifyWarning(SHOW_DANGER);
		digitalWrite(LED_TIPS_DARK, HIGH);
		tone(LED_TIPS_DARK, HARDWARE_BUSZZER_FREQUENCY);
		break;
	case TOO_CLOSE:
		notifyWarning(SHOW_DANGER);
		digitalWrite(LED_TIPS_DISTANCE, HIGH);
		tone(LED_TIPS_DISTANCE, HARDWARE_BUSZZER_FREQUENCY);
		break;
	case CLOSE_AND_DARK:
		notifyWarning(TOO_CLOSE);
		notifyWarning(TOO_DARK);
		//因为上面两个已经把小绿帽摘下，所以这儿就不用加代码了
		break;
	case CLEAR_DARK:
		digitalWrite(LED_TIPS_DARK, LOW);
		noTone(LED_TIPS_DARK);
		break;
	case CLEAR_CLOSE:
		digitalWrite(LED_TIPS_DISTANCE, LOW);
		noTone(LED_TIPS_DISTANCE);
		break;
	case CLEAR_ALL:
		notifyWarning(CLEAR_DARK);
		notifyWarning(CLEAR_CLOSE);
		notifyWarning(SHOW_SAFE); //这个地方一定要点起绿帽，因为是清除全部效果
		noTone(LED_TIPS_DISTANCE);
		noTone(LED_TIPS_DARK);
		break;
	case SHOW_DANGER:
		digitalWrite(LED_TIPS_SAFE, LOW);
		break;
	case SHOW_SAFE:
		digitalWrite(LED_TIPS_SAFE, HIGH);
		break;
	default:
		break;
	}
	return;
}


void setup() {
	Serial.begin(9600);
	pinMode(LED_TIPS_DARK, OUTPUT);
	pinMode(LED_TIPS_DARK, OUTPUT);
	pinMode(LED_TIPS_SAFE, OUTPUT);
	digitalWrite(LED_TIPS_SAFE, HIGH);
}

boolean bFlagForWarning[2] = { false,false };// Waring 维护列表，第一下标为 TOO_DARK  第二下标 TOO_CLOSE

//************************************
// Method:   setbFlagEffection
//					设置全局变量 bFlagForWarning[2] 实际影响效果
// FullName:  void setbFlagEffection()
// Returns:  boolean (true: 存在一个警告装置被启动 false:SAFE灯亮起
// Parameter: Null
//************************************
boolean setbFlagEffection() {
	if (bFlagForWarning[0] || bFlagForWarning[1]) { 
		if (bFlagForWarning[0]) {//过暗
			notifyWarning(NOTIFYWARNING_LIST::TOO_DARK);
		}else {
			notifyWarning(NOTIFYWARNING_LIST::CLEAR_DARK);
		}

		if (bFlagForWarning[1]) {//过近
			notifyWarning(NOTIFYWARNING_LIST::TOO_CLOSE);
		}
		else {
			notifyWarning(NOTIFYWARNING_LIST::CLEAR_CLOSE);
		}
		return true;
	}
	
	notifyWarning(NOTIFYWARNING_LIST::CLEAR_ALL);
	return false;

}
void loop() {
	int iCheckCounter;
	/* 分别连续检查 3 次，只要有一次为不成立则判断为 fake
	 * 降低敏感度*/
	for (iCheckCounter = 0; iCheckCounter < 3; ++iCheckCounter) {
		bFlagForWarning[0] = checkDark();
		if(!bFlagForWarning[0]){
			break;
		}
		delay(500);
	}
	for (iCheckCounter = 0; iCheckCounter < 3; ++iCheckCounter) {
		bFlagForWarning[1] = checkDistance(10);
		if (!bFlagForWarning[1]) {
			break;
		}
		delay(500);
	}
	if (setbFlagEffection()) { 
		delay(500);
		return;
	}
	delay(5000);
}

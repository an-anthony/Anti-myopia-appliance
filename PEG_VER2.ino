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
				����ɽ���ӣ�
				Bug Խ��Խ�٣�
*/

#define HARDWARE_INFRARED_DISTANCER A1 // �������� Pin ��
#define HARDWARE_PHOTORESISTER A0 // �������� Pin ��
#define LED_TIPS_DARK 4 // ���߹���ʱ����ʾLED Pin ��
#define LED_TIPS_DISTANCE 7 // �������ʱ����ʾ LED Pin ��
#define LED_TIPS_SAFE 2 // û��ʲôë��ʱ�� LED Pin ��

#define HARDWARE_PHOTORESISTER_LOWLIGHT 900 //��ѹ���ڸ�ֵʱ�ж�Ϊ �������ȡ�
#define HARDWARE_BUSZZER_FREQUENCY 1000 // ������Ƶ��
typedef enum {
	TOO_DARK, // ����
	TOO_CLOSE,  // ����
	CLOSE_AND_DARK, // ����������
	SHOW_SAFE,// ������ȫ����յС�̵�~[�ڲ�]
	SHOW_DANGER,//ժ����ñ��[�ڲ�]
	CLEAR_DARK, // �������Ч��
	CLEAR_CLOSE, // �������Ч��
	CLEAR_ALL // ���Ŀǰ����״̬
} NOTIFYWARNING_LIST;


//************************************
// Method:   checkDark
//					 ����Ƿ���߹������������裩
// FullName:  boolean checkDark() 
// Returns:   boolean (����(����ֵ > HARDWARE_PHOTORESISTER_LOWLIGHT)��true)
// Parameter: Null
//************************************
boolean checkDark() {

  Serial.println(analogRead(HARDWARE_PHOTORESISTER));
	return  analogRead(HARDWARE_PHOTORESISTER) > HARDWARE_PHOTORESISTER_LOWLIGHT;
}


//************************************
// Method:   checkDistance
//					 ���������(GP2Y0A21YK0F)�����Ƿ�С��ָ��ֵ
// FullName:  boolean checkDistance(int minDistance)
// Returns:   boolean (С��: true)
// Parameter: 
//						unsigned int minDistance			�û�ָ���ľ���
//************************************
boolean checkDistance(unsigned int minDistance) {
	double dVolt = (double) analogRead(HARDWARE_INFRARED_DISTANCER) * 5.00 / 1023.00;
	double dDistance,dDistanceRabbit;
	// �˺������ڶ��������⡣
	// �� y ֵ�ﵽ 3.3x (V) ��ʱ��delta = nan �����ǲ��ܵ���ĵ�
	// ����Ҳ��֪��Ϊʲô���ܵ�����Ǻ�������ʽ�������ģ�
	// ��˶��� 3.3x (V) ʱ�����У���ʱ����ֵ����ֱ�Ӵ��8cm
	if (abs(dVolt - 3.38) <= 0.1) {
		return 8 < minDistance;
	}

	double delta = sqrt(-545.16*dVolt + 1781.8169);
	double denominator = 2 * dVolt + 1.226 ;
	
	dDistance = (-delta + 46) / denominator;// https://wenku.baidu.com/view/cfbd075f312b3169a451a46e.html
	dDistanceRabbit = ( delta + 46 ) / denominator;

Serial.println(dVolt);
Serial.println(dDistanceRabbit);
	// ��Ϊ�˺��������ڶ��κ�����
	// ���������ֵ
	// ����ʵ���������ʱӦ�ÿ��� > 5 �� x ֵ
	if (dDistance >  5) { 
		return dDistance < minDistance;
	} else { 
		return dDistanceRabbit < minDistance;
	}
	
}

//************************************
// Method:   notifyWarning
//					����ʹ���ߵ�ǰ������������
// FullName:  void notifyWarning(int warningType) 
// Returns:   Null
// Parameter: 
//						int warningType			�������� (NOTIFYWARNING_LIST)
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
		//��Ϊ���������Ѿ���С��ñժ�£���������Ͳ��üӴ�����
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
		notifyWarning(SHOW_SAFE);
		noTone(LED_TIPS_DISTANCE);
		noTone(LED_TIPS_DARK);
		//����ط�һ��Ҫ�ӣ���Ϊ�����ȫ��Ч��
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

boolean bFlagForWarning[2] = { false,false };// Waring ά���б���һ�±�Ϊ TOO_DARK  �ڶ��±� TOO_CLOSE

//************************************
// Method:   setbFlagEffection
//					����ȫ�ֱ��� bFlagForWarning[2] ʵ��Ӱ��Ч��
// FullName:  void setbFlagEffection()
// Returns:  boolean (true: ����һ������װ�ñ����� false:SAFE������
// Parameter: Null
//************************************
boolean setbFlagEffection() {
	if (bFlagForWarning[0] || bFlagForWarning[1]) { 
		if (bFlagForWarning[0]) {//����
			notifyWarning(NOTIFYWARNING_LIST::TOO_DARK);
		}else {
			notifyWarning(NOTIFYWARNING_LIST::CLEAR_DARK);
		}

		if (bFlagForWarning[1]) {//����
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
	/* �ֱ�������� 3 �Σ�ֻҪ��һ��Ϊ���������ж�Ϊ fake
	 * �������ж�*/
	for (iCheckCounter = 0; iCheckCounter < 3; ++iCheckCounter) {
		bFlagForWarning[0] = checkDark();
		if(!bFlagForWarning[0]){
			break;
		}
		delay(500);
	}
	for (iCheckCounter = 0; iCheckCounter < 3; ++iCheckCounter) {
		bFlagForWarning[1] = checkDistance(21);
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

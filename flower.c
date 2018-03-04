// ������ "������" ��� ������������ ���� ESP32
// ���������: [0]:����� �����, [1]:������� ������, [2]:������� �������, 
// [3]:����� ������, [4]:������ ���., [5]:����� ���., [6]:PWM ���., 
// [7]:������. PWM, [8]:����� ���.����, [9]:����� ���. ���.,
// [10]:����� ����. ����, [11]:����� ����. ���.
// ���������� ����������: 4 ��.
// valdes[0] - ���������� ������� ���
// valdes[1] - ������� ��� � %
// valdes[2] - ���������� �������� �� �������
// valdes[3] - ������� ������� ���

void startfunc(){  // ����������� ���� ��� ��� ������ ������.
valdes[0] = 0;
valdes[2] = 0;
// ����� ���������
if(sensors_param.cfgdes[7]){valdes[3] = 4095;}
else{valdes[3] = 0;}
valdes[1]=0; 
change_pwm();
}

void timerfunc(uint32_t  timersrc) {// ��� 1 �������
  if(timersrc%60==0){   // ��� � ������
    int32_t time_start = 0;
    int32_t time_real = 0;
    int32_t time_stop = 0;
    time_start = sensors_param.cfgdes[8] * 60 + sensors_param.cfgdes[9];
    time_real = time_loc.hour * 60 + time_loc.min;
    time_stop = sensors_param.cfgdes[10] * 60 + sensors_param.cfgdes[11];
    if(time_start < time_real && time_real < time_stop){  
      valdes[2] = 1;     // ��������� �������� ��������� �� �������
    }
    else{               
      if(valdes[1]>0){  // ������ ��������� ���� ������� ������ ����
        if(sensors_param.cfgdes[7]){valdes[3] = 4095;}
        else{valdes[3] = 0;}
        valdes[1]=0;
        valdes[2]=0;
        change_pwm();
      }
    }
    if(sensors_param.cfgdes[6] && valdes[2]){   // ���� �������� ������ PWM
      if(bh_l<sensors_param.cfgdes[2] || bh_l>sensors_param.cfgdes[1]){ // ������� � �������� ��������� �������������
        valdes[3] = convertRange(bh_l, sensors_param.cfgdes[1], sensors_param.cfgdes[2], 4095, 0);
        valdes[3] = minRangeMax(valdes[3], 0, 4095);
        valdes[1]=convertRange(valdes[3], 0, 4095, 0, 100);
        if(sensors_param.cfgdes[7]){valdes[3] = 4095 - valdes[3];}
        change_pwm();
      }
      if(bh_l>sensors_param.cfgdes[2] && valdes[1]>0){ // ������� ������ ��������� � ������� ������ �������� 
        if(sensors_param.cfgdes[7]){valdes[3] = 4095;}
        else{valdes[3] = 0;}
        valdes[1]=0;
        change_pwm();
      }
      if(bh_l<sensors_param.cfgdes[1] && valdes[1]<100){ // ������� ������ �������� � ������� PWM ������ ���������  
        if(sensors_param.cfgdes[7]){valdes[3] = 0;}
        else{valdes[3] = 4095;}
        valdes[1]=100;
        change_pwm();
      }
    }
    if(sensors_param.cfgdes[5]){   // ���� �������� ������ ������     
      if(analogRead(0)>sensors_param.cfgdes[0] && digitalRead(22)){ // ��� ����� ����� � ���� � �����
        digitalWrite(19,1);
        delay(sensors_param.cfgdes[3]);
        digitalWrite(19,0);
      }
    }
  } 
  vTaskDelay(1000 / portTICK_PERIOD_MS);
}

void change_pwm() { // ��������� ������� ���������
  int32_t green = 0;
  int32_t ch_pwm = 0;
  int32_t status_pwm = 0;
  int32_t rgb_pwm[3] = {0,0,0};
  if(sensors_param.cfgdes[7]){green = 4095;}
  if(valdes[0] > valdes[3]){ // ���� ������� ������ �������
    ch_pwm = valdes[0] - valdes[3];
    status_pwm = valdes[0];
    valdes[0] = valdes[3];
    for(int32_t i=0; i<ch_pwm; i++){ // ������� ��������� �������
      status_pwm--;
      rgb_pwm[0] = status_pwm;
      if(!sensors_param.cfgdes[4]){
        rgb_pwm[1] = green;
      }
      else{rgb_pwm[1] = status_pwm; }
      rgb_pwm[2] = status_pwm;
      analogWrite(0,rgb_pwm[0]);
      delay(2);
      analogWrite(1,rgb_pwm[1]); 
      delay(2);
      analogWrite(2,rgb_pwm[2]);
      delay(15);
    }
  }
  if(valdes[0] < valdes[3]){ // ���� ������� ������ �������
    ch_pwm = valdes[3] - valdes[0];
    status_pwm = valdes[0];
    valdes[0] = valdes[3];
    for(int32_t i=0; i<ch_pwm; i++){ // ������� ��������� �������
      status_pwm++;
      rgb_pwm[0] = status_pwm;
      if(!sensors_param.cfgdes[4]){
        rgb_pwm[1] = green;
      }
      else{rgb_pwm[1] = status_pwm; }
      rgb_pwm[2] = status_pwm;
      analogWrite(0,rgb_pwm[0]);
      delay(2);
      analogWrite(1,rgb_pwm[1]); 
      delay(2);
      analogWrite(2,rgb_pwm[2]);
      delay(15);
    }
  }
}

int32_t convertRange(int32_t x, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max){ // ��������� ���������
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

int32_t minRangeMax(int32_t x, int32_t in_min, int32_t in_max){ // ����������� ���������
	if (x > in_max) {x = in_max;}
	if (x < in_min) {x = in_min;}
	return x;
}

void webfunc(char *pbuf) { // ����� ������ �� ������� ������
  os_sprintf(HTTPBUFF,"���������: %d lx<br>",bh_l);
  os_sprintf(HTTPBUFF,"���������: <progress value='%d' max='100'></progress> %d%%<br>",valdes[1],valdes[1]);
  if(analogRead(0)<sensors_param.cfgdes[0]){os_sprintf(HTTPBUFF,"��������� ����� <b><font color='green'>� �����</font></b><br>");}
  else{os_sprintf(HTTPBUFF,"��������� ����� <b><font color='red'><blink>������</blink></font></b><br>");}
  if(!digitalRead(22)){os_sprintf(HTTPBUFF,"<b><font color='red'>���� � ���� <blink>�����������!</blink></font></b><br>");}
  os_sprintf(HTTPBUFF,"<hr><br>");
  if(valdes[2]){
    os_sprintf(HTTPBUFF,"<a><div class='g_1 k fll' style='width:120px;background:green'>���������</div></a>");
  }
  else{
    os_sprintf(HTTPBUFF,"<a><div class='g_1 k fll' style='width:120px;background:grey'>���������</div></a>");
  }
  if(sensors_param.cfgdes[5]){
    os_sprintf(HTTPBUFF,"<a><div class='g_1 k fll' style='width:120px;background:green'>�����</div></a>");
  }
  else{
    os_sprintf(HTTPBUFF,"<a><div class='g_1 k fll' style='width:120px;background:grey'>�����</div></a>");
  }
  if(sensors_param.cfgdes[4]){
    os_sprintf(HTTPBUFF,"<a><div class='g_1 k fll' style='width:120px;background:green'>������ ���.</div></a><br>");
  }
  else{
    os_sprintf(HTTPBUFF,"<a><div class='g_1 k fll' style='width:120px;background:grey'>������ ���.</div></a><br>");
  }
  os_sprintf(HTTPBUFF,"<br><b>����� ������ ���������:</b>");
  os_sprintf(HTTPBUFF,"<b> � %d:%d �� %d:%d </b><br>",sensors_param.cfgdes[8],sensors_param.cfgdes[9],sensors_param.cfgdes[10],sensors_param.cfgdes[11]);
  os_sprintf(HTTPBUFF,"<hr>");
  os_sprintf(HTTPBUFF,"ADC %d<br>",analogRead(0));
  os_sprintf(HTTPBUFF,"Free memory: %d b.<br>",system_get_free_heap_size());
  os_sprintf(HTTPBUFF,"����������� ������: %d ����, %d:%d:%d <hr>",timer_uptime.day,timer_uptime.hour,timer_uptime.min,timer_uptime.sec);
}
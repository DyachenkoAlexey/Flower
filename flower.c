// ������ "������" ��� ������������ ���� ESP32
// ���������: [0]:����� �����, [1]:������� ������, [2]:������� �������,
// [3]:����� ������, [4]:������ ���., [5]:����� ���., [6]:PWM ���.,
// [7]:������. PWM, [8]:����� ���.����, [9]:����� ���. ���.,
// [10]:����� ����. ����, [11]:����� ����. ���., [12]:�������� ����� ������ ���.
// ���������� ����������: 7 ��.
// valdes[0] - ���������� ������� ���
// valdes[1] - ������� ��� � %
// valdes[2] - ���������� �������� �� �������
// valdes[3] - ������� ������� ���
// valdes[4] - ����� ����� ���������� ������
// valdes[5] - �������� �� ������ � web-��������
// valdes[6] - �������������� ��������� ��������� �� �������

void startfunc(){  // ����������� ���� ��� ��� ������ ������.
  //sensors_param.cfgdes[12]=sensors_param.cfgdes[12]+1;SAVEOPT
  valdes[2] = 0; // ���������� �������� ��������� �� �������
  valdes[4] = 0; // ����� ����� ���������� ������
  valdes[5] = 0; // ���������� ������� ������
  // ����� ���������
  if(sensors_param.cfgdes[7]){
    for(int8_t i=0; i<3; i++){
      analogWrite(i,4095);
      delay(2);
    }
  }
  else{
    for(int8_t i=0; i<3; i++){
      analogWrite(i,0);
      delay(2);
    }
  }
  valdes[3]=0;   // ������� ��� ���������
  valdes[0]=0;   // ������� ������� ���������
  valdes[1]=0;   // ������� ��������� � ���������
  valdes[6]=0;   // �������������� ��������� ��������� �� �������
}

void timerfunc(uint32_t  timersrc) {// ��� 1 �������
  if(valdes[5]){                    // ��������� ������� ������ �� web-��������
    int32_t buf = valdes[5]/1000000;
    switch (buf) {
      case 1:
        sensors_param.cfgdes[6]=valdes[5]%1000000;SAVEOPT
      break;
      case 2:
        sensors_param.cfgdes[5]=valdes[5]%1000000;SAVEOPT
      break;
      case 3:
        sensors_param.cfgdes[4]=valdes[5]%1000000;SAVEOPT
      break;
      case 4:
        sensors_param.cfgdes[8]=valdes[5]%1000000;SAVEOPT
      break;
      case 5:
        sensors_param.cfgdes[9]=valdes[5]%1000000;SAVEOPT
      break;
      case 6:
        sensors_param.cfgdes[10]=valdes[5]%1000000;SAVEOPT
      break;
      case 7:
        sensors_param.cfgdes[11]=valdes[5]%1000000;SAVEOPT
      break;
      case 8:
        if(digitalRead(22)){ // ��� ���� � �����
          valdes[4]=0;
          digitalWrite(19,1);
          delay(sensors_param.cfgdes[3]);
          digitalWrite(19,0);
        }
      break;
      case 9:
        valdes[6]=valdes[5]%1000000;
      break;
    }
    valdes[5]=0;
  }
  if(timersrc%60==10){   // ��� � ������
    static int32_t sum_en=0;
    valdes[4]++;
    int32_t time_start = 0;
    int32_t time_real = 0;
    int32_t time_stop = 0;
    time_start = sensors_param.cfgdes[8] * 60 + sensors_param.cfgdes[9];
    time_real = time_loc.hour * 60 + time_loc.min;
    time_stop = sensors_param.cfgdes[10] * 60 + sensors_param.cfgdes[11];
    if(time_start < time_real && time_real < time_stop){
      valdes[2] = 1;     // ��������� �������� ��������� �� �������
      valdes[6] = 0;     // ���������� �������������� ��������� �� �������
    }
    else{  
      if(valdes[6]){valdes[2]=1;}  // �������������� ������ ��������� �� �������
      else{valdes[2]=0;}           // �������������� ������ ��������� �� �������
    }
    if(sensors_param.cfgdes[6] && valdes[2]){   // ���� �������� ������ PWM ��� ������ �� �������
      int32_t bridges = 0;
      bridges = analogRead(3);
      if(bridges<sensors_param.cfgdes[2] || bridges>sensors_param.cfgdes[1]){ // ������� � �������� ��������� �������������
        valdes[3]=convertRange(bridges, sensors_param.cfgdes[1], sensors_param.cfgdes[2], 4095, 0);
        valdes[3]=minRangeMax(valdes[3], 0, 4095);
        valdes[1]=convertRange(valdes[3], 0, 4095, 0, 100);
      }
      if(bridges>=sensors_param.cfgdes[2]){ // ������� ������ ���������
        valdes[3]=0;
        valdes[1]=0;
      }
      if(bridges<=sensors_param.cfgdes[1]){ // ������� ������ ��������
        valdes[3]=4095;
        valdes[1]=100;
      }
    }
    else{ // ��� ���������� ������ ���������
      valdes[3] = 0;
      valdes[1]=0;
    }
    if(sensors_param.cfgdes[5] && sensors_param.cfgdes[12]<valdes[4]){
         // ���� �������� ������ ������ � ����� ����� �������� ����� ���������� ������
      if(digitalRead(22)){ // ��� ���� � �����
        if(analogRead(0)>sensors_param.cfgdes[0]){sum_en++;} // ��� ����� �����
        else{sum_en=0;}
        if(sum_en >= 2 && valdes[2]){ // � ������ �������  ������ ��������� � ����������� ������� ��������� 2 ���� ������
          valdes[4]=0;
          digitalWrite(19,1);
          delay(sensors_param.cfgdes[3]);
          digitalWrite(19,0);
          sum_en=0;
        }
      }
    }
    if(!sensors_param.cfgdes[5]){valdes[4]=sensors_param.cfgdes[12];}
  }
  change_pwm();
  vTaskDelay(1000 / portTICK_PERIOD_MS);
}

void change_pwm() { // ��������� ������� ���������
  int32_t bufpwm = 0;
  int32_t green = 0;
  int32_t rgb_pwm[3] = {0,0,0};
  if(sensors_param.cfgdes[7]){green = 4095;}
  if(valdes[0] > valdes[3]){ // ���� ������� ������ �������
    if(valdes[0]-valdes[3]>20){valdes[0]=valdes[0]-20;}
    else{valdes[0]=valdes[3];}
      if(sensors_param.cfgdes[7]){bufpwm = 4095 - valdes[0];}
      else{bufpwm = valdes[0];}
      rgb_pwm[0] = bufpwm;
      if(!sensors_param.cfgdes[4]){rgb_pwm[1] = green;} // ���� �������� ������
      else{rgb_pwm[1] = bufpwm;}
      rgb_pwm[2] = bufpwm;
      analogWrite(0,rgb_pwm[0]);
      delay(2);
      analogWrite(1,rgb_pwm[1]);
      delay(2);
      analogWrite(2,rgb_pwm[2]);
      delay(2);
  }
  if(valdes[0]<valdes[3]){ // ���� ������� ������ �������
    if(valdes[3]-valdes[0]>20){valdes[0]=valdes[0]+20;}
    else{valdes[0]=valdes[3];}
      if(sensors_param.cfgdes[7]){bufpwm = 4095 - valdes[0];}
      else{bufpwm = valdes[0];}
      rgb_pwm[0] = bufpwm;
      if(!sensors_param.cfgdes[4]){rgb_pwm[1] = green;}
      else{rgb_pwm[1] = bufpwm;}
      rgb_pwm[2] = bufpwm;
      analogWrite(0,rgb_pwm[0]);
      delay(2);
      analogWrite(1,rgb_pwm[1]);
      delay(2);
      analogWrite(2,rgb_pwm[2]);
      delay(2);
  }
  if(valdes[0]==valdes[3]){ // ���� ������� ����� �������
      if(!sensors_param.cfgdes[4]){rgb_pwm[1] = green;}
      else{
        if(sensors_param.cfgdes[7]){bufpwm = 4095 - valdes[0];}
        else{bufpwm = valdes[0];}
        rgb_pwm[1] = bufpwm;
      }
      analogWrite(1,rgb_pwm[1]);
      delay(2);
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
  os_sprintf(HTTPBUFF,"���������: %d (0-4095)<br>",adc1_get_raw(3));
  os_sprintf(HTTPBUFF,"��������� �������: <progress value='%d' max='100'></progress> %d%%<br>",valdes[1],valdes[1]);
  //os_sprintf(HTTPBUFF,"��������� �������: <progress value='%d' max='4095'></progress> %d<br>",valdes[0],valdes[0]);
  if(analogRead(0)<sensors_param.cfgdes[0]){os_sprintf(HTTPBUFF,"��������� ����� <b><font color='green'>� �����</font></b> (%d)<br>",analogRead(0));}
  else{os_sprintf(HTTPBUFF,"��������� ����� <b><font color='red'><blink>������</blink></font></b> (%d)<br>",analogRead(0));}
  if(!digitalRead(22)){os_sprintf(HTTPBUFF,"<b><font color='red'>���� � ���� <blink>�����������!</blink></font></b><br>");}
  os_sprintf(HTTPBUFF,"����� ����� ���������� ������ : %d �����<br>",valdes[4]);
  
  if(valdes[6]){
    os_sprintf(HTTPBUFF,"<button type='button' onclick='func(9, 0);repage()' style='width:195px;height:20px;color:#FFF;background:#00FF00'><b>���� ��������� ������</b></button>");
  }
  else{
    os_sprintf(HTTPBUFF,"<button type='button' onclick='func(9, 1);repage()' style='width:195px;height:20px'><b>��� ��������� ������</b></button>");
  }
  
  os_sprintf(HTTPBUFF,"<button type='button' onclick='func(8, 0);repage()' style='width:195px;height:20px'><b>������ ���� ���</b></button>");
    os_sprintf(HTTPBUFF,"<hr>");
    os_sprintf(HTTPBUFF,"<b>�������� �������:</b><br>");
  if(sensors_param.cfgdes[6]){
    os_sprintf(HTTPBUFF,"<button type='button' onclick='func(1, 0);repage()' style='width:130px;height:20px;color:#FFF;background:#00FF00'><b>��������� ����</b></button>");
  }
  else{
    os_sprintf(HTTPBUFF,"<button type='button' onclick='func(1, 1);repage()' style='width:130px;height:20px'><b>��������� ����</b></button>");
  }
  if(sensors_param.cfgdes[5]){
    os_sprintf(HTTPBUFF,"<button type='button' onclick='func(2, 0);repage()' style='width:130px;height:20px;color:#FFF;background:#00FF00'><b>����� ����</b></button>");
  }
  else{
    os_sprintf(HTTPBUFF,"<button type='button' onclick='func(2, 1);repage()' style='width:130px;height:20px'><b>����� ����</b></button>");
  }
  if(sensors_param.cfgdes[4]){
    os_sprintf(HTTPBUFF,"<button type='button' onclick='func(3, 0);repage()' style='width:130px;height:20px;color:#FFF;background:#00FF00'><b>������ ���</b></button>");
  }
  else{
    os_sprintf(HTTPBUFF,"<button type='button' onclick='func(3, 1);repage()' style='width:130px;height:20px'><b>������ ����</b></button>");
  }
  os_sprintf(HTTPBUFF,"<div class='c'><div class='main'><pre><table name='table1'border='0'class='catalogue'><b>����� ������ ���������:</b><tr style='background-color: yellow'><td> ���� </td><td> ������ </td><td> ���� </td><td> ������</td></tr>");
  os_sprintf(HTTPBUFF,"<tr><td>c <INPUT size=2 NAME='cfg8'id='cfg8'value='%02d'></td><td> <INPUT size=2 NAME='cfg9'id='cfg9'value='%02d'></td><td> �� <INPUT size=2 NAME='cfg10' id='cfg10' value='%02d'></td><td><INPUT size=2 NAME='cfg11'id='cfg11'value='%02d'></td></tr></table><br>",sensors_param.cfgdes[8],sensors_param.cfgdes[9],sensors_param.cfgdes[10],sensors_param.cfgdes[11]);
  os_sprintf(HTTPBUFF,"<button type='button' onclick='func2()'style='width:100px;height:20px'><b>�������</b></button></pre></div>");


  os_sprintf(HTTPBUFF,"<script>var request = new XMLHttpRequest();");
  os_sprintf(HTTPBUFF,"function reqReadyStateChange(){if(request.readyState == 4){var status = request.status;if (status == 200) {document.getElementById('output').innerHTML=request.responseText+ ' wait data save...';}}}");
  os_sprintf(HTTPBUFF,"function func(confset, valset){valset=parseInt(valset);valset=confset*1000000+valset;request.open('GET', 'valdes?int=5&set='+valset, true);request.onreadystatechange = reqReadyStateChange;request.send();}");
  os_sprintf(HTTPBUFF,"function func2(){func(4, cfg8.value);setTimeout('func(5, cfg9.value);', 2000);setTimeout('func(6, cfg10.value);', 4000);setTimeout('func(7, cfg11.value);', 6000);setTimeout('repage();', 6000);}");
  os_sprintf(HTTPBUFF,"function repage(){setTimeout('location.reload(true);', 2000);}</script>");

  //os_sprintf(HTTPBUFF,"<b> � %d:%d �� %d:%d </b><br>",sensors_param.cfgdes[8],sensors_param.cfgdes[9],sensors_param.cfgdes[10],sensors_param.cfgdes[11]);
  os_sprintf(HTTPBUFF,"<hr>");
  //os_sprintf(HTTPBUFF,"ADC %d<br>",analogRead(0));
    os_sprintf(HTTPBUFF,"<div id='output'></div>");
 // os_sprintf(HTTPBUFF,"<noscript><meta http-equiv='REFRESH' content='60'></noscript>");
//  os_sprintf(HTTPBUFF,"����������� ������: %d ����, %d:%d:%d <hr>",timer_uptime.day,timer_uptime.hour,timer_uptime.min,timer_uptime.sec);
}
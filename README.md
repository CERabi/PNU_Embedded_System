### 1. 프로젝트 소개
초음파 센서를 활용한 자율주행 RC 카
- 수업시간에 배운 여러 센서 및 보드의 기능을 이용하여 하드웨어를 개발한다.
- 블루투스 및 통신 관련 기능을 이용하여 하드웨어를 개발한다.
- 사람이 들어갈 수 없는 좁은 공간을 스스로 탐색 또는 조작할 수 있는 소형 자동주행 시스템을 개발한다.
<br/>

### 2. 동작 시나리오
1) 동작 선택
- 전원이 인가되면 조작 모드를 선택할 때까지 대기한다.
- 블루투스로 연결된 스마트폰 터미널을 통해 조작 모드를 자동 주행모드로 할 것인지, 
수동 주행 모드로 할 것인지 결정한다.
2) 자동 주행모드
- 초음파 센서를 통해 각 센서와 장애물 간의 거리를 측정한다.
- 측정한 거리 값을 바탕으로 주행상태(전진, 후진, 좌/우회전, 정지)를 결정한다.
3) 수동 주행모드
사용자의 스마트폰으로부터(터미널의 버튼 기능 사용) 전진, 후진, 좌우조향 조작을 입력
받는다.
입력된 값을 바탕으로 조작을 결정한다.
4) LCD
현재 조작 모드(자동주행/수동주행)를 LCD 모듈에 표시한다. 
5) 진동 센서
충돌이 감지될 시, 부저 경고음을 출력하며 시스템이 5 초간 정지한다.
충돌이 감지된 상태에서 조작 모드가 자동 주행 모드일 시, 수동 주행모드로 전환한다
<br/>

### 3. 시연 영상
[![Video Label](https://i9.ytimg.com/vi/Kj_wo4fQtwI/mqdefault.jpg?sqp=CMCH27cG-oaymwEoCMACELQB8quKqQMcGADwAQH4AYwCgALgA4oCDAgAEAEYciBaKDswDw==&rs=AOn4CLB-jemeWmMWzKxFEmJ1QR52WolSyA)](https://www.youtube.com/shorts/Kj_wo4fQtwI)
<br/>

### 4. 사용 기술
| 이름                  | 버전    |
|:---------------------:|:-------:|
| C    		| MSVC 2019   |
<br/>

### 5. 디렉토리 구조
```
├── Week 03/
├── Week 04/
├── Week 06/
├── Week 07/
├── Week 09/
├── Week 10/
├── Week 11/
├── Week 12/
├── Term/
├── 발표/
├── 실험 제공 뭐시기/
```
<br/>

### 6. 조원 소개
| 하규승 | 강준혁 | 박지원 | 이광훈(드랍) |
|:-------:|:-------:|:-------:|:-------:|
|<img width="100px" alt="MEMBER1" src="https://avatars.githubusercontent.com/u/174900093?v=4&size=64" /> | <img width="100px" alt="MEMBER2" src="https://github.com/pnuswedu/SW-Hackathon-2024/assets/34933690/fe4e8910-4565-4f3f-9bd1-f135e74cb39d" /> | <img width="100px" alt="MEMBER3" src="https://github.com/pnuswedu/SW-Hackathon-2024/assets/34933690/675d8471-19b9-4abc-bf8a-be426989b318" /> | | <img width="100px" alt="MEMBER4" src="https://github.com/pnuswedu/SW-Hackathon-2024/assets/34933690/675d8471-19b9-4abc-bf8a-be426989b318" /> |
| hgs6417@pusan.ac.kr | - | - | - |

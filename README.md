# 인디게임을 위한 커뮤니티/통계 사이트

# + 멀티 슈팅 게임

## 목차

- 서비스 개요

- 팀원 소개

- 기술 스택

- 시스템 아키텍처

- 기능 소개

- 게임 시연 영상

- 프로젝트 산출물

## 서비스 개요

```
훌륭한 인디게임이라도 홍보할 기회가 부족해 주목받지 못하고 사라지는 경우가 많습니다.
이러한 문제를 해결하기 위해 인디게임 개발자와 플레이어를 직접 연결해주는 플랫폼을 만들었습니다.


기간: 2024.10 ~ 2024.11 (5주)
```

## 팀원 소개

| <img title="" src="https://avatars.githubusercontent.com/hyoseon1201" alt="" width="300" height="200"> | <img title="" src="https://avatars.githubusercontent.com/CHAFALL" alt="" width="300" height="200"> | <img title="" src="https://avatars.githubusercontent.com/K-Dongyoung" alt="" width="300" height="200"> |
|:------------------------------------------------------------------------------------------------------:|:--------------------------------------------------------------------------------------------------:|:------------------------------------------------------------------------------------------------------:|
| [곽효선<br/>(Game Server)](https://github.com/hyoseon1201)                                                | [전근렬<br/>(Game Client)](https://github.com/CHAFALL)                                                | [김동영<br/>(Web)](https://github.com/K-Dongyoung)                                                        |

## 기술 스택

![](./readme-assets/기술%20스택.png)

```
Back-end : Spring boot 3.3.5, java 17
Front-end : Node.js 20 LTS, React 18.3.1
Game-Server : .Net 8.0
Game-Client : Unreal Engine 5.4 
```

## 시스템 아키텍처

<img title="" src="./readme-assets/아키텍처.png" alt="메인 페이지.png" width="340">

## 기능 소개

#### 웹

##### 메인페이지

| ![메인 페이지.png](./readme-assets/메인%20페이지.png) |
|:-------------------------------------------:|
| 메인페이지                                       |

##### 게시글

| ![게시글 위.png](./readme-assets/게시글%20위.png)   |
|:-------------------------------------------:|
| 게시글 위                                       |
| ![게시글 아래.png](./readme-assets/게시글%20아래.png) |
| 게시글 아래                                      |
| ![게시글 작성.png](./readme-assets/게시글%20작성.png) |
| 게시글 작성                                      |
| ![게시판 상세.png](./readme-assets/게시판%20상세.png) |
| 게시글 상세                                      |

##### 통계

| ![일별 게임 이용 시간.png](./readme-assets/일별%20게임%20이용%20시간.png) |
|:---------------------------------------------------------:|
| 일일 게임 이용 시간 (유저)                                          |
| ![일일 게임 접속자 수.png](./readme-assets/일일%20게임%20접속자%20수.png) |
| 일일 게임 접속자 수(전체)                                           |
| ![맵별 통계.png](./readme-assets/맵별%20통계.png)                 |
| 맵 별 통계 (FPS 특화 통계)                                        |

#### 게임

##### 로비 / 방 (TCP 서버 이용)

| ![방 생성.png](./readme-assets/방%20생성.png) | ![방 내부.png](./readme-assets/방%20내부.png) |
|:---------------------------------------:|:---------------------------------------:|
| 방 생성                                    | 방 내부 (채팅 기능 포함)                         |

##### 인게임 내 UI

| ![Tab.png](./readme-assets/Tab.png)     |
|:---------------------------------------:|
| Tab 키                                   |
| ![상점.png](./readme-assets/상점.png)       |
| 상점 (매 라운드 사이)                           |
| ![결과 창.png](./readme-assets/결과%20창.png) |
| 결과 창                                    |

###### 

###### 인게임 내 상세 동작은 아래 링크를 확인해주세요.

[인게임 내 상세 동작](https://github.com/CHAFALL/Blaster) 

### 게임 시연 영상

| ![시연-영상-_online-video-cutter.com_.gif](./readme-assets/게임%20시연%20영상.gif) |
|:------------------------------------------------------------------------:|
| 시연 영상 (1분 25초)                                                           |

### 프로젝트 산출물

##### ERD

![결과 창.png](./readme-assets/ERD.png)

##### 와이어프레임

![결과 창.png](./readme-assets/와이어프레임.png)

https://www.figma.com/design/aLG09eCO3Mz2DcafxBr0yE/Blaster?node-id=0-1&p=f&t=YoF8Je5eyPRvfqci-0

# U2605\_C\_Industry

\# 제빵 공장 디지털 트윈 시뮬레이션



> \*\*Unreal Engine 5.5 / C++\*\* — 산업 공정의 물류 흐름을 실시간으로 시각화하는 관제형 시뮬레이션



!\[Unreal Engine](https://img.shields.io/badge/Unreal%20Engine-5.5-0E1128?logo=unrealengine\&logoColor=white)

!\[C++](https://img.shields.io/badge/C%2B%2B-17-00599C?logo=cplusplus\&logoColor=white)

!\[Niagara](https://img.shields.io/badge/VFX-Niagara-orange)

!\[HISM](https://img.shields.io/badge/Rendering-HISM%20Instancing-green)



<!-- 대표 GIF: 전체 공정이 한눈에 보이는 장면 (상품 흐름 + 설비 가동 + 대시보드) -->

!\[전체 공정 데모](Docs/Images/overview.gif)



원재료(밀가루)가 창고에서 출고되어 컨베이어를 따라 흐르고, 가공 설비를 거치며

\*\*밀가루 → 반죽 → 빵 → 크림빵\*\*으로 변환되는 전 과정을 실시간 시각화합니다.

모든 설비는 클릭 한 번으로 상태를 확인하고, 가동 중에도 파라미터를 조정할 수 있습니다.



<br>



\## 🎬 데모 영상



\[!\[데모 영상 썸네일](Docs/Images/video\_thumbnail.png)](https://youtu.be/영상링크)



> 클릭하면 전체 시연 영상(약 N분)으로 이동합니다.



<br>



\## ✨ 핵심 기능



\### 1. 컨베이어 네트워크 — 그리드 기반 자동 연결



!\[컨베이어 흐름](Docs/Images/conveyor\_flow.gif)



\- 컨베이어를 레벨에 배치하면 \*\*그리드 좌표(FIntVector) 매칭\*\*으로 토폴로지가 자동 구성됩니다.

\- 연결 관계는 \*\*그래프 자료구조(`UCConveyorGraph`)\*\* 로 관리되며, 입구 노드(Entry)·다음 노드(Next)·도착 지점(Sink)을 빌드 타임에 한 번 계산합니다.

\- 상품은 0.5초 주기의 이산 시뮬레이션으로 스플라인 위를 이동합니다.



\### 2. Niagara 기반 상품 시각화 — Actor 스폰 없는 대량 표현



!\[단계별 메시 교체](Docs/Images/product\_stages.gif)



\- 상품을 Actor로 스폰하지 않고, \*\*Niagara 파티클 위치 배열\*\*(`SetVectorArrayUserParameter`)로 표현합니다.

\- \*\*Mesh Renderer + MeshIndex 바인딩\*\*으로 가공 단계에 따라 파티클 메시가 교체됩니다 (밀가루 포대 → 반죽 → 빵 → 크림빵).

\- 상품 수가 늘어나도 드로우콜·GC 부담이 거의 없는 구조입니다.



\### 3. 클릭 인터랙션 + What-if 파라미터 조정



!\[정보 UI와 슬라이더](Docs/Images/info\_ui.gif)



\- 설비/창고 클릭 시 \*\*정보 UI 팝업\*\* + 외곽선 하이라이트 (`IIClickable` 인터페이스 기반).

\- 가공 설비의 \*\*처리 시간을 슬라이더로 실시간 조정\*\* — 가공 중이면 다음 사이클부터 반영하고, 변경 내역은 이벤트 로그에 기록됩니다.

\- 창고의 자동 출고 주기 조정으로 병목 실험이 가능합니다.



\### 4. 실시간 관제 대시보드 + 생산 리포트



!\[대시보드](Docs/Images/dashboard.gif)



\- 월드 내 전광판(`ACInfoBoard`)에 \*\*총 생산량 / 분당 처리량 / 경과 시간 / 설비 가동률\*\*(OEE Availability 단순화)을 1초 주기로 갱신합니다.

\- 시뮬레이션 시작·정지, 재고 소진, 파라미터 변경 등 주요 이벤트를 \*\*타임스탬프 로그\*\*로 표시합니다.

\- 생산 통계는 \*\*CSV로 내보내기\*\* (한글 컬럼, UTF-8 BOM, 최근 5개 파일 순환 보관 + 재시작 시 인덱스 자동 복원).



<br>



\## 🧠 기술적 의사결정



| 결정 | 이유 |

|---|---|

| 상품을 Actor 대신 \*\*Niagara 위치 배열\*\*로 시각화 | 관제 시각화는 수백\~수천 개 객체를 다뤄야 함. Actor 스폰 방식은 드로우콜·GC 비용이 객체 수에 비례하지만, 파티클 배열 갱신은 비용이 거의 일정함 |

| \*\*0.5초 타이머 기반 이산 시뮬레이션\*\* (Tick 미사용) | 프레임레이트와 무관한 결정적(deterministic) 진행 보장. 시뮬레이션 비용의 상한이 명확하고, 일시정지/재개 구현이 단순해짐 |

| 정적 설비 메시를 \*\*HISM으로 통합 관리\*\* (`UCInstancedMeshSubsystem`) | 동일 메시를 사용하는 설비를 인스턴스 하나로 묶어 드로우콜 최소화. `PerInstanceCustomData`로 설비별 가동 상태(발광)를 머티리얼에 전달 |

| 컨베이어 토폴로지를 \*\*그래프 자료구조\*\*로 분리 | 시뮬레이션 로직(`FConveyorSimulator`)과 연결 정보(`UCConveyorGraph`)를 분리해 책임을 명확히 함. 향후 동적 배치 시 그래프 재빌드만으로 대응 가능 |

| \*\*Subsystem 분할 + 델리게이트 통신\*\* | 설비↔UI 직접 참조를 제거. 데이터 흐름(IO)과 UI 알림을 별도 GameInstanceSubsystem으로 분리해, 위젯·설비·통계가 서로의 존재를 모른 채 동작 |

| 시뮬레이션 경과 시간의 \*\*단일 출처\*\*(`UCSimulationTimeSubsystem`) | 가동률·분당 처리량 등 모든 통계가 같은 시계를 참조하도록 강제. 일시정지 시간 보정도 한 곳에서만 처리 |



<br>



\## 🏗 아키텍처



```mermaid

flowchart LR

&#x20;   subgraph Actors\["설비 (Actor)"]

&#x20;       ST\[Storage<br/>창고]

&#x20;       PR\[Processor<br/>가공 설비]

&#x20;   end



&#x20;   subgraph GI\["GameInstance Subsystems"]

&#x20;       IO\[CommunicationSubsystem\_IO<br/>상품 데이터 흐름]

&#x20;       UI\[CommunicationSubsystem\_UI<br/>UI 이벤트 허브]

&#x20;   end



&#x20;   subgraph World\["World Subsystems"]

&#x20;       CV\[ConveyorSubsystem<br/>그래프 + 시뮬레이터]

&#x20;       HISM\[InstancedMeshSubsystem<br/>HISM 통합]

&#x20;       STAT\[ProductionStatSubsystem<br/>생산 통계]

&#x20;       TIME\[SimulationTimeSubsystem<br/>경과 시간]

&#x20;   end



&#x20;   subgraph View\["시각화"]

&#x20;       NG\[Niagara<br/>상품 파티클]

&#x20;       WG\[Widgets<br/>정보 UI / 대시보드 / 로그]

&#x20;   end



&#x20;   ST -- ProductStarted --> IO

&#x20;   IO --> CV

&#x20;   CV -- 위치/메시 인덱스 배열 --> NG

&#x20;   CV -- 도착 시 DeliverProductTo --> IO

&#x20;   IO -- ReceiveProduct --> PR

&#x20;   PR -- 가공 완료 → ProductStarted --> IO

&#x20;   ST \& PR -- 상태 브로드캐스트 --> UI

&#x20;   STAT -- 대시보드 데이터 --> UI

&#x20;   UI --> WG

&#x20;   ST \& PR -- 메시 등록/CustomData --> HISM

&#x20;   STAT -- 경과 시간 조회 --> TIME

```



\*\*데이터 흐름 한 줄 요약:\*\*

창고 출고 → `IO` 브로드캐스트 → `ConveyorSubsystem`이 그래프 위에서 상품 이동 → Niagara 배열 갱신 → 도착 시 `IO`를 통해 설비에 전달 → 가공 후 다시 출고. UI는 이 흐름을 `UI` Subsystem을 통해서만 구독합니다.



<br>



\## 📂 프로젝트 구조



```

Source/U2605\_C/

├── BaseSystem/          # GameMode, PlayerController, Pawn (입력/카메라)

├── Conveyor/            # 컨베이어 액터, 그래프, 시뮬레이터

├── ProductionEquipment/ # 창고(Storage), 가공 설비(Processor)

├── Communication/       # IO/UI 통신 Subsystem (델리게이트 허브)

├── MeshInstancing/      # HISM 통합 관리 Subsystem

├── ProductionStat/      # 생산 통계 집계 + CSV 익스포터

├── SimulationTime/      # 시뮬레이션 경과 시간 단일 출처

├── Component/           # Niagara 래퍼, 가공 로직, 화면 위젯 컴포넌트

├── Widget/              # 정보 UI, 대시보드, 이벤트 로그

├── Interface/           # IClickable, IProductReceiver

├── StructData/          # 상품/UI 데이터 구조체

└── Utilities/           # 조기 반환 매크로, 로그 헬퍼, 그리드 상수

```



<br>



\## 🚀 실행 환경



\- \*\*Unreal Engine 5.5\*\* (DX12, SM6)

\- Windows 10/11

\- 프로젝트 클론 후 `.uproject` 우클릭 → \*Generate Visual Studio project files\* → 빌드 → 에디터에서 `Level\_Basic3` 실행



<br>



\## 🗺 확장 로드맵



\- \*\*설비 동적 배치\*\* — 그리드 키 매칭 구조를 그대로 활용해 런타임 배치/연결 지원

\- \*\*외부 데이터 연동 (HTTP 폴링)\*\* — 실제 설비 데이터를 수신해 시뮬레이션에 반영하는 디지털 트윈 단계로 확장

\- 상품별 라우팅 분기 (한 출구 → 다중 입구 분배)


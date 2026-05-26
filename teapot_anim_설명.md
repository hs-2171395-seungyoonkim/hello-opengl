# 주전자 키프레임 애니메이션 코드 설명

## 제출 파일 목록

| 파일 | 역할 |
|---|---|
| `src/teapot_anim.cpp` | 메인 애니메이션 코드 (새로 작성) |
| `shaders/14.2.teapot.vs` | 주전자 버텍스 셰이더 (기존 파일 재사용) |
| `shaders/14.2.teapot.fs` | 주전자 프래그먼트 셰이더 (기존 파일 재사용) |
| `shaders/14.2.lamp.vs` | 광원 버텍스 셰이더 (기존 파일 재사용) |
| `shaders/14.2.lamp.fs` | 광원 프래그먼트 셰이더 (기존 파일 재사용) |

---

## 과제 요구사항 체크

| 요구사항 | 충족 여부 | 비고 |
|---|---|---|
| 주전자 모델 사용 | ✅ | `teapot.vbo` 로드 |
| 키프레임 3개 이상 | ✅ | **4개** 설정 |
| 쿼터니언으로 회전 | ✅ | `glm::angleAxis`, `glm::quat_cast`, `glm::mix` (SLERP) |
| 이동 포함 (가산점 30%) | ✅ | `glm::mix`로 위치도 보간 |
| 윈도우 타이틀에 학번+이름 | ✅ | `"2171395 seungyoonkim"` |

---

## 코드 구조 설명

### 1. 기본 설정 (teapot.cpp와 동일)

```cpp
Camera camera(glm::vec3(0.0f, 0.5f, 4.0f));
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
```

카메라와 광원 위치는 원본 `teapot.cpp`와 동일하게 유지했습니다.
셰이더도 기존 `14.2.teapot.vs / fs`를 그대로 사용합니다.

---

### 2. 베이스 회전 쿼터니언

```cpp
glm::quat qBase = glm::angleAxis(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
```

주전자 `.vbo` 파일은 기본적으로 X축 방향으로 누워 있습니다.
`-90°` X축 회전으로 주전자를 세워주는 베이스 회전입니다.
원본 `teapot.cpp`에서 `glm::rotate(model, glm::radians(-90.0f), ...)` 하던 것을 쿼터니언으로 표현한 것입니다.

---

### 3. 키프레임 4개 정의

| 키프레임 | 추가 회전 | 위치 | 설명 |
|---|---|---|---|
| KF0 | 없음 (0°) | (0, 0, 0) | 정면, 원점 |
| KF1 | Y축 120° | (1.5, 0.5, 0) | 오른쪽으로 이동, Y축 회전 |
| KF2 | Y축 180° + Z축 45° 기울기 | (0, 1.2, -0.5) | 위쪽으로 이동, 기울어진 상태 |
| KF3 | Y축 300° + X축 30° 기울기 | (-1.5, 0.5, 0) | 왼쪽으로 이동, 앞뒤 기울기 |

```cpp
// KF0: 추가 회전 없음 — glm::angleAxis 방식 (j11 코드 참고)
glm::quat qRot0 = glm::angleAxis(glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

// KF1: Y축 120도 — glm::angleAxis 방식
glm::quat qRot1 = glm::angleAxis(glm::radians(120.0f), glm::vec3(0.0f, 1.0f, 0.0f));

// KF2: Y축 180도 + Z축 45도 — glm::quat_cast 방식 (j11 코드 참고)
glm::mat4 m2 = glm::mat4(1.0f);
m2 = glm::rotate(m2, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
m2 = glm::rotate(m2, glm::radians(45.0f),  glm::vec3(0.0f, 0.0f, 1.0f));
glm::quat qRot2 = glm::quat_cast(m2);

// KF3: Y축 300도 + X축 30도 — glm::quat_cast 방식
glm::mat4 m3 = glm::mat4(1.0f);
m3 = glm::rotate(m3, glm::radians(300.0f), glm::vec3(0.0f, 1.0f, 0.0f));
m3 = glm::rotate(m3, glm::radians(30.0f),  glm::vec3(1.0f, 0.0f, 0.0f));
glm::quat qRot3 = glm::quat_cast(m3);
```

> `j11.quaternion.cpp`에서 두 가지 쿼터니언 생성 방식(`glm::angleAxis`와 `glm::quat_cast`)을 모두 사용한 것을 그대로 따라갔습니다.

---

### 4. 최종 키프레임 쿼터니언 합성

```cpp
glm::quat kfQ[NUM_KF] = {
    qRot0 * qBase,
    qRot1 * qBase,
    qRot2 * qBase,
    qRot3 * qBase
};
```

GLM 쿼터니언 곱셈 `A * B`는 행렬처럼 **"B 먼저 적용, 그 다음 A"** 순서입니다.
따라서 `qRot * qBase` = **먼저 베이스 회전(-90° X)으로 주전자를 세우고, 그 위에 키프레임 회전을 추가** 하는 것입니다.

---

### 5. 렌더 루프에서의 보간 (SLERP + LERP)

```cpp
// 현재 어느 세그먼트(구간)에 있는지 계산
float animTime = fmod(currentFrame, TOTAL_DURATION); // 전체 시간을 주기로 나머지
int seg     = (int)(animTime / SEG_DURATION);         // 현재 키프레임 인덱스
float t     = (animTime - seg * SEG_DURATION) / SEG_DURATION; // 구간 내 진행도 [0,1]
int nextSeg = (seg + 1) % NUM_KF;                    // 다음 키프레임 인덱스

// 회전: SLERP (Spherical Linear Interpolation)
// glm::mix는 쿼터니언에 대해 SLERP를 수행 — j11.quaternion.cpp와 동일한 방식
glm::quat q   = glm::mix(kfQ[seg], kfQ[nextSeg], t);

// 이동: LERP (Linear Interpolation) — 가산점 30%
glm::vec3 pos = glm::mix(kfPos[seg], kfPos[nextSeg], t);
```

- `glm::mix(q1, q2, t)` : 쿼터니언 SLERP. t=0이면 q1, t=1이면 q2. 중간은 구면 선형 보간.
- `glm::mix(v1, v2, t)` : vec3 LERP. 일반 선형 보간.
- **`j11.quaternion.cpp`에서 `glm::mix(q0, q1, t)`를 사용한 것과 완전히 동일한 방식.**

---

### 6. 모델 행렬 구성

```cpp
glm::mat4 model = glm::translate(glm::mat4(1.0f), pos) * glm::mat4_cast(q);
```

- `glm::mat4_cast(q)` : 보간된 쿼터니언을 4x4 회전 행렬로 변환
- 최종 행렬 = **이동(T) × 회전(R)** 순서
- 버텍스에 적용: `model * vertex` = T × R × vertex → 원점에서 회전 후 위치로 이동

---

### 7. 타임라인 요약

```
시간(초):  0        2        4        6        8(=0으로 반복)
          KF0 -----> KF1 -----> KF2 -----> KF3 -----> KF0
위치:    원점      오른쪽     위쪽뒤     왼쪽      원점
회전:     정면    Y120°     Y180°+Z45°  Y300°+X30°  정면
```

세그먼트당 2초, 총 8초 주기로 반복됩니다.

---

## SLERP vs LERP 차이

| | SLERP (회전) | LERP (이동) |
|---|---|---|
| 대상 | 쿼터니언 | vec3 위치 |
| 경로 | 구면(Sphere) 위의 호 | 직선 |
| 특징 | 일정한 각속도로 자연스러운 회전 | 일정한 속도로 직선 이동 |
| GLM 함수 | `glm::mix(quat, quat, t)` | `glm::mix(vec3, vec3, t)` |

LERP로 쿼터니언을 보간하면 회전 속도가 일정하지 않아 어색해 보입니다.
SLERP는 구면 위를 일정 각속도로 이동하므로 자연스러운 회전 애니메이션을 만들 수 있습니다.

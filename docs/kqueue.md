# kqueue

`kernel event notification machanism`  

[kqueue FREEBSD소스코드 주소](https://github.com/freebsd/freebsd-src/blob/main/sys/kern/kern_event.c)

```c
int
kqueue(void);

int
kevent(
		int kq, const struct kevent *changelist, int nchanges,
		struct kevent *eventlist, int nevents, const struct timespec *timeout
		);

EV_SET(&kev, ident, filter, flags, fflags, data, udata);
```

## kqueue() 시스템콜

kqueue()함수는 fd를 할당한다. 이 fd는 `filter`라고 하는 작은 커널 코드 결과에 기반해 `kevent가 발생하거나, 조건이 유지되는 경우` user에게 알리는 일반적인 방법을 제공한다.  


### kevent

```c
struct kevent {
             uintptr_t       ident;          /* identifier for this event */
             int16_t         filter;         /* filter for event */
             uint16_t        flags;          /* general flags */
             uint32_t        fflags;         /* filter-specific flags */
             intptr_t        data;           /* filter-specific data */
             void            *udata;         /* opaque user data identifier */
     };
```

kevent는 `ident, filter, 추가적인 udata value의  tuple`로 식별된다.  
이는 해당 튜플에 대해 통지할 대상 조건을 지정한다.  
(ident, filter, 추가적인 udata value) 튜플은 `지정된 kqueue에 한 번만` 나타날 수 있습니다.  
`지정된 kqueue에 대해 동일한 튜플을 등록`하려는 이후의 시도는 추가가 아니라 `감시되는 조건을 대체`하게 됩니다.  
udata 값이 튜플의 일부로 간주되는지 여부는 키 이벤트의EV_UDATA_SPECIFIC 플래그에 의해 제어됩니다.  

<br>

### filter?

filter는 이벤트를 감시하기 전에 초기 등록 시에 실행되어 미리 존재하는 조건이 있는지 감지합니다.  
그리고 이벤트가 평가될 때마다 filter가 실행되어 이벤트 조건을 검사합니다.  
filter가 조건을 만족시킨다면, 해당 이벤트는 kqueue에 등록되어 유저가 이를 추출하도록 합니다.

<br>

`즉, filter는 이벤트를 감시하는데, filter의 조건이 만족되면 해당 이벤트를 kqueue에 등록하여 유저가 이를 추출할 수 있도록 하는 역할`을 합니다.  
`filter는 초기 등록 시와 이벤트가 평가될 때마다 실행되며, 이를 통해 이벤트를 감시하고 처리`할 수 있습니다.


<br>

중요한 것은 filter는 user가 kqueue로부터 kevent를 받으려고 시도할때 작동합니다.  
만약 filter가 더이상 존재하지 않는 이벤트를 발생시키는 조건을 나타낸다면, kevent는 kqueue로부터 삭제되고, 반환되지 않는다.  


filter를 작동하는 다수의 event는 kqueue에 다수의 event를 만들지 않는다. 대신, `filter는 event를 1개의 kevent 구조체로 합친다.`  
fd에 대한 close호출은 해당 fd를 참조하는 모든 kevent를 삭제한다.  



<br>


### fork하면?

`kqueue 함수에 의해 반환되는 kernel event queue는 fork에 의해 자식 프로세스에 상속되지 않습니다.`  

<br>

### kevent 시스템콜

```c
int
kevent(
		int kq, const struct kevent *changelist, int nchanges,
		struct kevent *eventlist, int nevents, const struct timespec *timeout
		);
```

kevent 시스템콜은  
1. queue에 event를 등록할때 사용
2. user에 pending 상태의 event를 반환한다.

- 인자설명
	- `changelist` : kevent배열의 포인터
		- changelist에 포함된 모든 변화는 보류중인 event가 queue로부터 읽어지기 전에 적용된다.
	- `nchanges` : changelist의 크기
	- `eventlist` : 읽어올 데이터를 저장할 kevent의 배열 포인터를 의미.
	- `nevents` : eventlist의 크기
	- `timeout` : non-NULL 포인터일때, event를 대기하는 최대 대기시간을 정의합니다. `NULL포인터라면 kevent()함수는 event가 발생할때까지 무한정 대기`합니다.




<br>

### EV_SET 매크로

- `kevent는 ident, filter, 추가적인 udata value의  tuple로 식별된다는 것을 기억하자.`
- 이 셋의 조합은 유일성을 만족해야하며, 겹치면 최근의 것으로 덮어씌워진다.

```c
EV_SET(&kev, ident, filter, flags, fflags, data, udata);
```

- EV_SET 매크로는 kevent 구조체 초기화를 쉽게하는 매크로.

- 인자설명
	- `kev` : 뒤의 인수를 설정한 event구조체의 주소
	- `ident` : event의 발생지를 식별하기위해 사용되는 값. 정확한 해석은 뒤에 추가된 filter나, fd에 의해 결정된다.
	- `filter` : 이 이벤트를 처리하기위해 사용되는 kernel filter를 식별한다. system에서 정의된 filter는 후술.
	- `flags` : event에 수행할 행동을 의미한다.
	- `fflags` : filter를 위한 flags
	- `data` : filter위한 data value
	- `udata` : 고유성 판단값. 이벤트의 유일성을 보장하기 위해 이벤트를 식별하는 여러 요소 중 하나.

- EV_SET의 flags에 사용될 flag들
	1. EV_ADD
		- kqueue에 event를 추가한다. 이미 존재하는 event를 다시 더하면, 원래 event를 덮어씌운다.
		- EV_DISABLE flag가 덮어씌워지지 않는다면 event추가하면 자동으로 활성화한다.
	2. EV_ENABLE
		- 만약 event가 트리거되면 kevent함수가 event를 반환하도록 한다.
	3. EV_DISABLE
		- kevent를 비활성화해 kevent가 반환되지 않도록한다.
	4. EV_DELETE
		- kqueue로부터 event를 삭제한다. fd에 붙어있는 event들은 fd의 마지막 close에 따라 자동으로 삭제된다.
	5. EV_RECEIPT
		- kqueue에 많은 변화를 가할때, 보류중인 이벤트를 사용하지 않고 처리하는데에 유용하다.
		- input으로 전달되면, EV_ERROR가 항상 반환된다.
	6. EV_ONESHOT
		- 필터가 처음으로 트리거 될때만 반환되도록한다. kqueue로부터 유저가 event를 받으면 삭제된다.
	7. EV_CLEAR
		- user에 의해 event가 받아진 이후에 상태를 초기화한다.
		- 필터가 현재상태를 보고하는게 아니라 상태 전환을 보고하는 경우에 유용하다.
	8. EV_EOF
		- 필터링된 이벤트가 EOF조건을 만족하는 경우에 설정된다.
	9. EV_OOBAND
		- EV_OOBAND 플래그는 소켓의 읽기 필터에서 out-of-band 데이터가 있는지 여부를 나타내기 위해 설정될 수 있다. 
		- 이 플래그는 필터링된 이벤트가 out-of-band 데이터를 포함하는 경우 설정된다.
		- out-of-band 데이터는 일반적인 데이터와는 다르게 우선순위가 높은 데이터이다. 
		- 소켓에서 out-of-band 데이터를 전송하려면 일반적인 데이터 전송 방법 외에도 다른 방법을 사용해야 한다. 
		- out-of-band 데이터를 사용하려면 소켓에서 송신자와 수신자 간의 특별한 협약이 필요하다.
	

<br><br>

- filter의 종류


1. EVFILT_READ
	- fd를 식별자로 사용하며, fd의 데이터가 읽을 수 있다면, 반환한다.
	- 이 필터의 행동은 fd의 유형에 따라 달라진다.
	- socket인경우
		-  이전에 호출된 listen에 의해 만들어진 socket의 fd는 pending 상태의 connection 요청이 있을 경우에 반환된다.
		- 







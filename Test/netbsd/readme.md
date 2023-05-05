[netbsd kqueue tutorial](https://wiki.netbsd.org/tutorials/kqueue_tutorial/)  

- 이 문서는 kqueue의 모든 기능을 소개하는 것이 아닌, kqueue의 방법론을 소개합니다.  
- kqueue는 어플리케이션의 다양한 이벤트, 조건에 대한 관ㅅ미을 등록하고 이에 대한 알림을 효율적으로 전달하는 표준 API를 제공합니다. 확장성, 유연성, 신뢰성, 정확성을 제공하도록 설계되었습니다.  


# kqueue API

## kevent data structure

```cpp
struct kevent {
            uintptr_t ident;        /* identifier for this event */
            uint32_t  filter;       /* filter for event */
            uint32_t  flags;        /* action flags for kqueue */
            uint32_t  fflags;       /* filter flag value */
            int64_t   data;         /* filter data value */
            void     *udata;        /* opaque user data identifier */
};
```

## pair

- kevent는 `ident`와 `filter` 쌍 (+udata) 로 식별됩니다.  
- ident는 모니터링하고자하는 대상에 따라 file, socket, stream, pid 혹은 signal number가 될 수 있다.
- filter는 각 이벤트를 처리하는 데 사용되는 kernel filter를 식별한다.
- 미리 정의된 시스템 필터가 있으며, `EVFILT_READ, EVFILT_WRITE`와 같은 미리 정의된 일부 시스템 필터는 각각 읽기, 쓰기 작업을 위해 데이터가 존재할 때 트리거된다.


- 예를 들어 소켓에서 읽을 수 있는 데이터가 있을 때 알림을 받으려면 `<sckfd, EVFILT_READ>`형식으로 키 이벤트를 지정해야 한다. 
- 여기서 skfd는 소켓과 연결된 fd이다. 

- 프로세스의 활동을 모니터링하려면 `<pid, EVFILT_PROC>`튜플이 필요하다. 
- kqueue에는 pair가 동일한 키이벤트가 하나만 있을 수 있습니다.

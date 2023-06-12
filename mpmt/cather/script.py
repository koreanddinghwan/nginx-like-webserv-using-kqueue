from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.support import expected_conditions as EC
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common.action_chains import ActionChains
from selenium.common.exceptions import NoSuchElementException
import chromedriver_autoinstaller
import time
from datetime import datetime
from numba import jit


#브라우저 드라이버 인스톨
chromedriver_autoinstaller.install()
#브라우저 안꺼지게함
chrome_options = Options()
chrome_options.add_experimental_option("detach", True)

#크롬 드라이버에 옵션 적용하여 드라이버 실행 -> url주소 접근(내 프로젝트 디펜스 슬롯) -> 새창 가져옴
driver = webdriver.Chrome(options=chrome_options)
#url = 'https://projects.intra.42.fr/projects/cpp-module-04/slots?team_id=4819841'
url = 'url'
driver.get(url)


#login 링크 클릭 가능할때까지 대기 후 -> 클릭
wait = WebDriverWait(driver, 10)
login_link = wait.until(EC.element_to_be_clickable((By.XPATH, '/html/body/div[2]/div[2]/div/div[2]/div/div[3]/a[1]')))
login_link.click()

#id, pwd이 보일때까지 대기, send_keys뒤에 String으로 아이디, 비번 입력
id_input = wait.until(EC.visibility_of_element_located((By.XPATH,'//*[@id="username"]')))
id_input.send_keys("id")
pw_input = wait.until(EC.visibility_of_element_located((By.XPATH, '//*[@id="password"]')))
pw_input.send_keys("pw")

#Remember me 키게 할까..고민
remember = wait.until(EC.visibility_of_element_located((By.XPATH, '//*[@id="rememberMe"]')))

#login button 대기 후 클릭
login = wait.until(EC.visibility_of_element_located((By.XPATH, '//*[@id="kc-login"]')))
login.click()

target = driver.find_element(By.XPATH, '//*[@id="calendar"]/div[2]/div/table/thead/tr/td/div/table/thead/tr')
days = target.text.split(sep=' ')
today = str(datetime.today().weekday()+2)

#daysIndex = str(days.index(today[0:3])+1)
tmpTime = datetime.today().strftime("%I")
validTime = str(tmpTime) if int(tmpTime) >= 10 else str(tmpTime[1:2])
print(today)
#타겟으로 할 시 분 am, pm 입력
#hour = 0~12
#minutes = 00 or 30
#target = am or pm

targetHour = 7
targetMinutes = 00
target = 'pm'
maxTime = 5
def scoreConverter (tH, tM, sH, sM):
	return (((sH-tH)*60) + (sM - tM))

def slotFinder(th, tm, tt, slot):
	hour = th
	minutesArr = [0, 15, 30, 45]
	oo = tt
	if th > 23:
		return print("시간확인하세용")
	elif th > 12:
		if tt == 'pm':
			oo = 'am'
			hour += th - 12
		elif tt == 'am':
			oo = 'pm'
			hour += th - 12
	for minute in range(0, len(minutesArr)):
		if minute == 0:
			min = '00'
		else:
			min = str(minutesArr[minute])
		hourStr = str(hour)+':'+min+' '+oo.upper()
		try:
			_slot = slot.find_element(By.XPATH, "//*[contains(@data-full,"+"'"+hourStr+"'"+")]")
		except NoSuchElementException:
			continue
		else:
			return _slot
	return False
#아마.. 실제로는 이렇게 동작할 것 같은.

#스크롤 할 시간
strForMove = str(targetHour)+':'+'00'+' '+str(target)

#요일의 table data X path
slot_xPath = '//*[@id="calendar"]/div[2]/div/table/tbody/tr/td/div/div/div[3]/table/tbody/tr/td['+today+']'

#움직일 위치
moveXPath = "//span[contains(text(),"+"'"+strForMove+"'"+")]"

#웨잇..테스트인데, //contains로 하위에 어베일러블 있는지 바로 접근해도 될듯

#forWait = wait.untill(EC.presence_of_element_located((By.XPATH, slotEventContainer//)

while True:
	i = maxTime
	try:
		#slotParant = driver.find_element(By.XPATH, slot_xPath)
		#임플릭트 제거하고도 테스트 돌려봐야함.
		#driver.implicitly_wait(0.5)
		slotParant = wait.until(EC.presence_of_element_located((By.XPATH, slot_xPath)))
		slotEventContainer = slotParant.find_element(By.XPATH, slot_xPath+"//div[contains(text(),"+"fc-event-container"+")]")
		TargetSlots = slotEventContainer.find_elements(By.CLASS_NAME, "fc-content")
	except NoSuchElementException:
		driver.refresh()
	else:
		for slot in TargetSlots:
			i = 0
			for i in range(0, maxTime):
				print(slot.text)
				_slot = slotFinder(targetHour+i, targetMinutes, target, slot)
				if _slot:
					movePoint = driver.find_element(By.XPATH, moveXPath)
					_moveMiddle = slot.find_element(By.XPATH, "//div[contains(text(),"+"'"+"Available"+"'"+")]")
					ActionChains(driver)\
						.move_to_element(_moveMiddle)\
						.click(_moveMiddle)\
						.perform()
					_moveCancle = wait.until(EC.element_to_be_clickable((By.XPATH,'//*[@id="slotSelectionModalContainer"]/div/div/div[3]/button[1]')))
						#//*[@id="slotSelectionModalContainer"]/div/div/div[3]/button[2]
					ActionChains(driver)\
						.move_to_element(_moveCancle)\
						.click(_moveCancle)\
						.perform()
					i = 99
					break
				else:
					continue
			if i == 99:
				userInput = input("계속 : r, 그만 x:\n")
				if userInput == 'r':
					i = maxTime
				elif userInput == 'x':
					i = 99
				break
	finally:
		if i == maxTime:
			driver.refresh()
		elif i == 99:
			driver.quit()
			break
	
		

#ActionChains(driver)\
#	.move_to_element(movePoint)\
#	.perform()

#최상단 요일별 인덱스 접근 -> 요일별 인덱스로 접근한 element의 fc-event-container 접근 ->해당 부모가 fc-content들고 있는지 확인
#1번 분기 -> content 없음 -> 새로고침
#있음 -> continer안에 content들의 data-time 보면서, 요청 시간 근처 있는지 확인
#2번 분기 -> 없음 -> 새로고침
#있음, 점수 환산 -> 점수 가장 낮은 fc-content의 Available로 무브
#
#//*[@id="calendar"]/div[2]/div/table/tbody/tr/td/div/div/div[3]/table/tbody/tr/td[3]/div/div[2]/a[2]/div[1]/div[1]
#//*[@id="calendar"]/div[2]/div/table/tbody/tr/td/div/div/div[3]/table/tbody/tr/td[3]/div/div[2]/a[2]/div[1]/div[2]
#slots = driver.find_element(By.XPATH, slot_xPath)

#td 2번이 보이는 테이블, td 3이 이제 실제로 평가가 존재하는 테이블
#예상 1. tr로 날짜를 구분한다? -> 화, 수, 두개를 구분해서 실제로 데이터가 어떻게 보이는지 보면 좋겠음(tr의 변경이 있는가)
#-> td가 0이된 것은 지난 날짜, 결국 daysIndex로 td 접근 하는게 맞았음




	

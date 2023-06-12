def slotFinder(th, tm, tt, slot):
	hour = th
	minutesArr = [0, 15, 30, 45]
	minIdx = minutesArr.index(tm)
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
	for minute in range(minIdx, len(minutesArr)):
		if minute == 0:
			min = '00'
		else:
			min = str(minutesArr[minute])
		hourStr = str(hour)+':'+min+' '+oo.upper()
		print(hourStr)
		try:
			if slot:
				_slot = 'tete'
			elif min == '30':
				_slot = 'cut'
			else:	
				_slot = 5/0
		except:
			continue
		else:
			return _slot
	return False
str = slotFinder(9, 00, 'pm', '')
print(str)
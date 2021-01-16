import json
import requests
import ftplib

filename = "mydomini.html"
ftpurl = "ftpperso.free.fr"
ftppath = "static_domini"
loginftp= "minbiocabanon"
passftp= "melisse1"
ftpcommand = 'STOR '+ filename +''

# Function that get ngrok url (https)
def get_ngrok_url():
	url = "http://localhost:4040/api/tunnels/"
	res = requests.get(url)
	res_unicode = res.content.decode("utf-8")
	res_json = json.loads(res_unicode)
	for i in res_json["tunnels"]:
		if i['name'] == 'command_line':
			#print(i['public_url'])
			return i['public_url']
			break

# ### Main Function  ###

# Get public IP @ from local ngrok request

try :
	# put ngrok url in html body
	htmlcode = "<html><head><meta http-equiv=\"refresh\" content=\"0; URL="+ get_ngrok_url() +"/\"></head></html>"
	print( htmlcode )
except:
	print("Error while recovring ngrok IP")
	
try:
	# add html redirection in index.html page
	with open(filename, 'w') as htmlpage:
		htmlpage.write(htmlcode)
		
	#upload html page on domini

	#connect to ftp server
	ftp = ftplib.FTP(ftpurl)
	ftp.login(loginftp, passftp)
	# select directory
	ftp.cwd(ftppath)

	#upload the file
	file = open(filename,'rb')			# file to send
	print("ftp command = {}".format(ftpcommand))
	ftp.storbinary(ftpcommand, file)	# send the file
	file.close()							# close file and FTP
	ftp.quit()
except:
	print("Error FTP while uploading html file")	
# end

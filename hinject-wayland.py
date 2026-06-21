import subprocess
import sys
import time
from InquirerPy import inquirer
from InquirerPy.validator import EmptyInputValidator
from selenium import webdriver
from selenium.webdriver.firefox.options import Options
from selenium.webdriver.common.by import By
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
print("[hinject bot: ] Running in firefox mode")
print("Initializing htype bot")
print("=================================================")
message = inquirer.text(message="[htype bot:] Typing text: ", validate=EmptyInputValidator("TEXT CANNOT BE EMPTY")).execute()
wpm = inquirer.text(message="[htype bot:] WPM: ", filter=lambda val: int(val) if val else 120).execute()
error = inquirer.text(message="[htype bot:] Error Rate: ", filter=lambda val: float(val) if val else 0.0).execute()
jitter = inquirer.text(message="[htype bot:] Jitter Rate: ", filter=lambda val: float(val) if val else 0.0).execute()

print("[hinject bot: ] Configuring firefox for monkeytype automation")
firefox_options=Options()
driver = webdriver.Firefox(options=firefox_options)
driver.get("https://monkeytype.com")
try:
    accept_cookies = WebDriverWait(driver, 4).until(
        EC.element_to_be_clickable((By.CLASS_NAME, "acceptAll"))
    )
    accept_cookies.click()
    time.sleep(1)
except:
    pass
print("[hinject bot:] Your wheels are in my hand just stay calm.")
try:
    body = driver.find_element(By.TAG_NAME, "body")
    body.send_keys(Keys.ESCAPE)  # Opens search overlay
    time.sleep(0.5)
    
    active_input = driver.switch_to.active_element
    active_input.send_keys("zen")
    time.sleep(0.3)
    active_input.send_keys(Keys.ENTER)  # Switches layout to Zen
    time.sleep(1)
except Exception as e:
    print("[Warning] Could not auto-toggle mode. Press 'Esc' and type 'zen' if needed.")

print("[hinject bot:] Click directly inside the blank Zen Mode typing canvas.")
print("WARNING: The htype bot will automatically pick up focus and begin typing after 5 seconds.")
time.sleep(5)

input_field = None
try:
    input_field = driver.find_element(By.ID, "wordsInput")
except:
    input_field = driver.switch_to.active_element

print("[hinject bot:] Injecting humanized character stream into target arena")

# Heredoc
cmd = f"""htype --wpm {wpm} --error {error} --jitter {jitter} << 'EOF'
{message}
EOF"""

process = subprocess.Popen(
    cmd,
    shell=True,
    stdout=subprocess.PIPE,
    stderr=sys.stderr,
    text=True,
    bufsize=0
)

try:
    while True:
        char = process.stdout.read(1)
        
        if not char and process.poll() is not None:
            break
            
        if char:
            # Map structural characters explicitly to browser actions
            if char == '\b':
                input_field.send_keys(Keys.BACKSPACE)
            elif char in ('\n', '\r'):
                input_field.send_keys(Keys.ENTER)
            else:
                input_field.send_keys(char)
                
except KeyboardInterrupt:
    print("Process interrupted by user.")
    process.terminate()

print("[htype bot and hinject bot:] Finished typing successfully. Keeping browser open for two minutes, see the overview!")
time.sleep(120)
driver.quit()

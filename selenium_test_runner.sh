# starting Selenium standalone
java -jar selenium-server-standalone-3.4.0.jar > selenium.log 2>&1 < /dev/null &
SELENIUM_PID=$!
echo $SELENIUM_PID > selenium.pid

# running our tests including Selenium driver ones
# python manage.py test

# Stopping Selenium
# something that is meant for running on a CI server
if [ -f selenium.pid ]; then
    echo "Shutting down Selenium"
    pkill -F selenium.pid
    while ps -p `cat selenium.pid`; do echo Waiting Selenium to finish; sleep 1; done;
    rm selenium.pid
else
   echo "Selenium not running ..."
fi

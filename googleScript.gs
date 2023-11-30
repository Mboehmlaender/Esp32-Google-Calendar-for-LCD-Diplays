/*

Credit to COERT VONK https://coertvonk.com/sw/embedded/esp8266-clock-import-events-from-google-calendar-15809 and kristiantm https://github.com/kristiantm/eink-family-calendar-esp32

I modified the scripts to fit my needs.

You need to put this up as a public script. Please note that this will expose your calendar entries to the internet - however, only you have the unique id - and there is no input or write access

*/

function doGet(e) {
  //var calendars = CalendarApp.getCalendarById('ID_OF_CALENDAR');
  //var cal = CalendarApp.getCalendarsByName('NAME_OF_CALENDAR')[0]; // 0 is subcalendar ID, mostly "0"
  //var cal = CalendarApp.getDefaultCalendar();
  //var calendars = CalendarApp.getCalendarsByName('NAME_OF_CALENDAR');
  
  if (calendars == undefined) {
    Logger.log("No data");
    return ContentService.createTextOutput("no access to calendar hubba");
  }
  
  var calendars_selected = [];
  
  for (var ii = 0; ii < calendars.length; ii++) {
    if (calendars[ii].isSelected()) {
      calendars_selected.push(calendars[ii]);
      Logger.log(calendars[ii].getName());
    }
  }
  
  Logger.log("Old: " + calendars.length + " New: " + calendars.length);
  var start = new Date(); //Start from now
  var stop = new Date(new Date().setHours(24,0,0,0)); //End on same day at midnight
  
  //var events = calendars.getEvents(start, stop,{max: 2}); //pull start/stop time
  var events = mergeCalendarEvents(calendars_selected, now, stop); //pull start/stop time
  
  
  var str = '';
  for (var ii = 0; ii <= 2; ii++){
    var event=events[ii]; 
    if(event)
    {   
    var myStatus = event.getMyStatus();
    
    console.log(event);
    // define valid entryStatus to populate array
    switch(myStatus) {
      case CalendarApp.GuestStatus.OWNER:
      case CalendarApp.GuestStatus.YES:
      case CalendarApp.GuestStatus.NO:  
      case CalendarApp.GuestStatus.INVITED:
      case CalendarApp.GuestStatus.MAYBE:
      default:
        break;
    }
  
    if(!event.isAllDayEvent()){
    // Show just every entry regardless of GuestStatus
    str += Utilities.formatDate(event.getStartTime(), Session.getScriptTimeZone(), "HH:mm") + ' - ' + Utilities.formatDate(event.getEndTime(), Session.getScriptTimeZone(), "HH:mm") + ';' +
    event.getTitle() +';';
    console.log(str);
    }
      }
  }
  return ContentService.createTextOutput(str);
}
function mergeCalendarEvents(calendars, startTime, endTime) {
  var params = { start:startTime, end:endTime, uniqueIds:[] };
  return calendars.map(toUniqueEvents_, params)
                  .reduce(toSingleArray_)
                  .sort(byStart_);
}

function toCalendars_(id) { return CalendarApp.getCalendarById(id); }

function toUniqueEvents_ (calendar) {
  return calendar.getEvents(this.start, this.end)
                 .filter(onlyUniqueEvents_, this.uniqueIds);
}

function onlyUniqueEvents_(event) {
  var eventId = event.getId();
  var uniqueEvent = this.indexOf(eventId) < 0;
  if(uniqueEvent) this.push(eventId);
  return uniqueEvent;
}

function toSingleArray_(a, b) { return a.concat(b) }
  function byStart_(a, b) {
    return a.getStartTime().getTime() - b.getStartTime().getTime();
}
var BrowserClient = require('./BrowserClient');
const { promisify } = require('util');
const sleep = promisify(setTimeout);
const path = '../../out/browser_project/browser';
var client = new BrowserClient(path,['-remote-debugging-port=12345','-view-port=1855,984','--user-agent=test user agent','-path=/tmp/','--lang=en-us','--accept-languages=en-US,en;q=0.9,es;q=0.8'],true);
var httpServerUrl = "http://192.168.6.222/barcode";
var httpServerDomain = `192.168.6.160`;
function preprocssCookie(msg){
   if(msg.cookies){
       
       for(var i = 0 ; i < msg.cookies.length ; i ++){
           var cookie =  msg.cookies[i];
           if(typeof cookie != "string") continue;
           var items = cookie.split(";");
           var data = {};
           for( var j = 0 ; j < items.length ; j++){
                 var item = items[j].trim();
                 var index = item.indexOf("=");
                 if(index < 0) continue;
                 if(j==0){
                     data.name =  item.substr(0,index);
                     data.value =  item.substr(index+1,item.length-index);
                 } else {
                     var filed = item.substr(0,index);
                     data[filed] = item.substr(index+1,item.length-index);
                 }
           }
           msg.cookies[i] = data;
       }
   }
   return msg;
}
var testMode = {
   open_url:true,
   get_screenshot:true,
   open_url_error:false,
   open_url_timeout:false,
   open_url_set_cookies:false,
   get_cookies:false,   
   inject_javascript:true,
   inject_javascript_preload:false,
   inject_javascript_timeout:false,
   freeze:false,
   interceptor:true
}
async function init(){

    var ret = await client.start();
    if(!ret.success) { console.log(result.error);return;}
    console.log(ret);
    

     var msg;
         
     ///// open url ///////////
     if(testMode.open_url){
     msg = { action:"open", url:`${httpServerUrl}`,timeout:60};
     
     ret = await client.sendMessage(msg);
     console.log("open-test",ret);
     }

     ///// get screnn shot  //////////
      if(testMode.get_screenshot){
      msg = { action:"get_screenshot"};
      await sleep(500);   
      ret = await client.sendMessage(msg);
      console.log("get-screenshot",ret);
      }


    ///// open url with timeout ////// 
    if(testMode.open_url_timeout){
    msg = { action:"open", url:`${httpServerUrl}/timeout`,timeout:20};
    
    ret = await client.sendMessage(msg);
    console.log("timeout-test",ret); 
    }

    ///// open url with 500 error //////
    if(testMode.open_url_error){
    msg = { action:"open", url:`${httpServerUrl}/error`,timeout:60};
    
    ret = await client.sendMessage(msg);
    console.log("500 error result",ret); 
    }
    
   ///// open url with set cookie  ////////////////
   if(testMode.open_url_set_cookies){
   msg = {
     action:"open",
     url:`${httpServerUrl}`,
     timeout:60,
     cookies:[`domain_cookie=aa8PewVCV7k68R2T; expires=Thu, 18-Dec-2100 12:00:00 GMT; domain=${httpServerDomain}; path=/`,"other_cookie=K42c3j43UfFLvBGX; expires=Thu, 18-Dec-2100 12:00:00 GMT; domain=srgarxkz2erygjt4.test.dug0.com; path=/"]
   }
   preprocssCookie(msg);
   // console.log(msg);
   
   ret = await client.sendMessage(msg);
   console.log("set cookie\n",ret); 
   }
  
   /////// get cookies ///////////
   if(testMode.get_cookies){
        msg = {
            action:"get_cookies"   
        }
        
        ret = await client.sendMessage(msg);
        console.log("get cookie\n",ret); 
   }


   ////// run javascript ////////////
   if(testMode.inject_javascript){
    msg = {
        action:"inject_javascript",
        timeout:1000, // timeout for the callback, milliseconds
        content:`(function(){ 
                       //// after loaded page, if get window.innerWidth, it return 0
                       setTimeout(function(){ cef_browser_callback(
                        {
                            'user-agent': navigator.userAgent,
                            'view-port':[window.innerWidth, window.innerHeight],
                            'languages':navigator.languages,
                            'language':navigator.language,
                            'cookies':document.cookies
                        });
                       },100);
                }())`
    }
    
    ret = await client.sendMessage(msg);
    console.debug("inject javascript\n",ret,ret.result.data["view-port"]); 

 }

 ////// run javascript preload////////////
 if(testMode.inject_javascript_preload){

    msg = {
        action:"open",
        url:`${httpServerUrl}preload`,
        preload:"(function(){ window._aj_preload_applied=true;})()", // arbitrary code executed in each frame before the content loads, no expect result
        timeout:60
    }
    
    ret = await client.sendMessage(msg);
    console.debug("inject javascript preload\n",ret); 


    msg = {
        action:"inject_javascript",
        timeout:500, // timeout for the callback, milliseconds
        content:`(function(){ 
                       //// after loaded page, if get window.innerWidth, it return 0
                        cef_browser_callback( window._aj_preload_applied_2 && document.querySelector("iframe").contentWindow._aj_preload_applied_2 );
                       
                }())`
    }
    
    ret = await client.sendMessage(msg);
    console.debug("inject javascript preloaded result\n",ret,ret.result); 

 }
 ////// run javascript timeout ////////////
 if(testMode.inject_javascript_timeout){
    msg = {
        action:"inject_javascript",
        timeout:1000, // timeout for the callback, milliseconds
        content:`(function(){ 
                    setTimeout(function(){ cef_browser_callback('result for injection1');},1500);
                }())`
    }
    
    ret = await client.sendMessage(msg);
    console.debug("inject javascript 1\n",ret); 
    msg = {
        action:"inject_javascript",
        timeout:3000, // timeout for the callback, milliseconds
        content:`(function(){ 
                    setTimeout(function(){ cef_browser_callback({'foo':'result for injection 4','large string':new Array(10).join("x")});},1500);
                }())`
    }
    
    ret = await client.sendMessage(msg);
    console.debug("inject javascript result 2\n",ret); 

 } 
 if(testMode.freeze){

    msg = { action:"freeze" };
    ret = await client.sendMessage(msg);
    console.log("freeze result",ret);  


    msg = {
        action:"inject_javascript",
        timeout:5000,
        content:"(function(){ window.location='index.htm'; } ) ()"
    }
    ret = await client.sendMessage(msg);
    console.log("freeze for result",ret);  

    msg ={
        action:"inject_javascript",
        timeout:1500,
        content:`(function(){ 
            
            document.querySelector('body').innerHTML = '<a id="btn" href="/">Hello world</a>'; 
            document.querySelector("#btn").addEventListener("click",function(e){
                e.preventDefault();
                this.innerHTML = "size will resized";
            });;
            document.querySelector("#btn").click();
            cef_browser_callback({
            url:window.location.href,
            content:document.querySelector('body').innerHTML,
            geometry: document.querySelector("#btn").getBoundingClientRect()

        }) } ) ()`
     }
    ret = await client.sendMessage(msg);
    console.log("freeze test result",ret,ret.result.data.geometry);  


 }
 if(testMode.interceptor){
    msg = {
        action:"get_requests"        
    }
    ret = await client.sendMessage(msg);
    console.debug("interceptor result\n",ret.result.data); 
   }
   msg = {
    action:"close"        
   }
   ret = await client.sendMessage(msg);
   console.debug("close result\n",ret); 

}
init();



const { spawn } = require('child_process');

const { promisify } = require('util')
const sleep = promisify(setTimeout)

class BrowserClient{
     constructor(path,argv,logging){
        this.path = path;
        this.argv = argv;
        this.logging = logging
     }
     async waitProcessing(){
       while(this.processing){
           await sleep(20);      
        
       }
     }
     async start(){

        var self = this; 
        this.processing = true; 

        this.process = spawn(this.path,this.argv);
        //console.log(this.argv)
        this.process.stdout.on('data', (data)=>{this.processMessage(data)});
        this.process.stderr.on('data', (data) => {
            if(self.logging)
            console.log(`stderr: ${data}`);
        });
        this.process.on('close', (code) => {
            console.log(`child process exited with code ${code}`);
            process.exit(0);
        });
        this.process.on('error', (error) => {
            self.response = {type:"start",success:false,error:error}
            self.processing = false;
        });
        var self = this;
        await this.waitProcessing();
        return this.response;
      }
      
      processMessage(data){
        
        data = data.toString();
        data = Buffer.from(data,'base64').toString();
        try{
            
            var msg = JSON.parse(data);
            switch(msg.type){
                case 'app-ready':
                {
                    this.response = {type:"start",success:true}
                    this.processing = false;                   
                    break;
                }
                default:    
                {
                    this.response = msg;
                    this.processing = false;
                }
            }
            return;
        }
        catch(e){
            console.warn(e)
            this.response = {result:"fail",message:e.message}
            this.processing = false;                   
            

        }    
    }

    async sendMessage(message){
        message = JSON.stringify(message);
        this.processing = true;
        var msg = Buffer.from(message).toString('base64');
        this.process.stdin.write(msg+"\n");
        await this.waitProcessing();
        return this.response;
    }

}


module.exports = BrowserClient;

package com.prototype.airswimmer;

public class AudioThread extends Thread{
	int playTime;
	
	public AudioThread(int playTime){
		super();
		this.playTime = playTime;
	}
	
	public void run( )
    {        		
       final float frequency = 440;
       float increment = (float)(2*Math.PI) * frequency / 44100; // angular increment for each sample
       float angle = 0;
       AndroidAudioDevice device = new AndroidAudioDevice();
       float samples[] = new float[1024];
       System.out.println("start making noise done!");
       
       
       if(playTime == 0){
	       while(!isInterrupted() )
	       {
	       	   for( int i = 0; i < samples.length; i++ )
	              {
	                 samples[i] = (float)Math.sin( angle );
	                 angle += increment;
	              }
	
	       	   device.writeSamples( samples );
	       }
       }else{
    	   long endTime = System.currentTimeMillis() + playTime *1000;
    	   
    	   while(!isInterrupted() && endTime >= System.currentTimeMillis()){
    		   for( int i = 0; i < samples.length; i++ )
	              {
	                 samples[i] = (float)Math.sin( angle );
	                 angle += increment;
	              }
	
	       	   device.writeSamples( samples );
    	   }   
       }
       
    }
}

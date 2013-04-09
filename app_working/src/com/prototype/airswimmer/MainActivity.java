package com.prototype.airswimmer;

import com.example.airswimmer.R;
import android.media.AudioTrack;
import android.os.Bundle;
import android.app.Activity;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;


public class MainActivity extends Activity {
	
	AudioTrack track;
	Thread t1 = null;
	short[] buffer = new short[1024];
	float samples[] = new float[1024];
	boolean play = false;
	 
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		
		Button createBtn = (Button) findViewById(R.id.stopBtn);
		createBtn.setOnClickListener(new OnClickListener() {
	 
		  public void onClick(View v) {
	                
			  EditText txt = (EditText) findViewById(R.id.status);
			  txt.setText("create pressed");
			  stopAudio();
		  }
		});
		
		Button playBtn = (Button) findViewById(R.id.playBtn);
		playBtn.setOnClickListener(new OnClickListener() {
	 
		  public void onClick(View v) {
	                
			  EditText txt = (EditText) findViewById(R.id.status);
			  txt.setText("play pressed");
			  playAudio();
		  }
		});
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}
	//method for creating an audio file
	public boolean stopAudio(){
		
		t1.interrupt();
		
	      EditText txt = (EditText) findViewById(R.id.status);
		  txt.setText("audioTrack stopped");
		return true;
	}
	//method to play the audio file , communiucation with aux port of mobile device
	public void playAudio(){
		System.out.println("start thread from main activity");
		EditText et = (EditText) findViewById(R.id.playTime);
		
		int playTime = Integer.parseInt(et.getText().toString());
		t1 = new AudioThread( playTime);
	    t1.start();
		EditText txt = (EditText) findViewById(R.id.status);
		txt.setText("audioTrack plays");
	}
	  
}


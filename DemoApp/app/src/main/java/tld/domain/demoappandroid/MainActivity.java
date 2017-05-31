package tld.domain.demoappandroid;

import android.app.Activity;
import android.app.NativeActivity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.TextView;

public class MainActivity extends Activity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        final LinearLayout linearLayout = new LinearLayout(this);
        final Button loadLibraryButton = new Button(this);
        final Button startNativeCodeButton = new Button(this);

        loadLibraryButton.setText("Load library");
        loadLibraryButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                try {
                    System.loadLibrary("native-lib");
                    loadLibraryButton.setEnabled(false);
                    startNativeCodeButton.setEnabled(true);
                } catch (UnsatisfiedLinkError e) {
                    // Failed to load native-lib to JVM.
                    e.printStackTrace();
                }
            }
        });

        startNativeCodeButton.setEnabled(false);
        startNativeCodeButton.setText("Start NativeActivity");
        startNativeCodeButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent(MainActivity.this, NativeActivity.class);
                MainActivity.this.startActivity(intent);
            }
        });

        linearLayout.addView(loadLibraryButton);
        linearLayout.addView(startNativeCodeButton);

        super.setContentView(linearLayout);
    }
}

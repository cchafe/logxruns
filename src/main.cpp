// nefariously adapted from https://github.com/jackaudio/example-clients/blob/master/simple_client.c
// to support various of CC's projects
// this one's useful for counting xruns
// run from src/ directory with
// ../build-logxruns-Desktop-Release/logxruns

#include <QCoreApplication>
#include <QDebug>
#include <QLoggingCategory>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <jack/jack.h>
jack_port_t *input_port;
jack_port_t *output_port;
#include <iostream>

using std::cout;
using std::endl;

double srate;
uint32_t FPP;
double PPS;
int xctr;
int cctr;
int dctr;
int pctr;
jack_client_t *client;

// Jack xrun callback function, called
// whenever there is a xrun.
static int xrun_callback ( void * )
{
    xctr++;
    return 0;
}

// report xrun info
void report()
{
    double tmp = (pctr/(double)PPS);
    float fDspLoad = jack_cpu_load(client);
    qDebug() << xctr << "\t(" << (int)(tmp) << ")" << "\t(" << (xctr/tmp) << ")" << "\t" << fDspLoad;
}

// report xrun info
void dummy()
{
//    fprintf(stderr, "\n");
    dctr++;
}

int
process (jack_nframes_t nframes, void *arg)
{
//    fprintf(stderr, ".");
    cctr++;
    dummy();
    //    jack_default_audio_sample_t *in, *out;

    //    in = (jack_default_audio_sample_t *)jack_port_get_buffer (input_port, nframes);
    //    out =(jack_default_audio_sample_t *) jack_port_get_buffer (output_port, nframes);
    //    memcpy (out, in,
    //        sizeof (jack_default_audio_sample_t) * nframes);
    if (!pctr) {
        qDebug() << "xruns" << "(" << "secs" << ")" << "(" << "avg xruns" << ")" << "" << "DspLoad";
    }
    pctr++;
    //qDebug() << ctr;
    int lazyReportInterval = 2*PPS; // secs
    if (!(pctr%lazyReportInterval)) {
        if (cctr != dctr) qDebug() << "cctr" << "(" << cctr << ")" << "dctr" << "(" << dctr << ")";
        cctr=0;
        dctr=0;
    }
    return 0;
}

void
jack_shutdown (void *arg)
{
    exit (1);
}

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    fprintf(stderr, "%s\n", localMsg.constData());
    fflush(stderr);
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QLoggingCategory::setFilterRules(QStringLiteral("*.debug=true"));
    qInstallMessageHandler(myMessageOutput);
    //    int
    //    main (int argc, char *argv[])
    {
        const char **ports;
        const char *client_name = "simple";
        const char *server_name = NULL;
        jack_options_t options = JackNullOption;
        jack_status_t status;

        pctr=0;
        xctr=0;
        cctr=0;
        dctr=0;
        client = jack_client_open (client_name, options, &status, server_name);
        if (client == NULL) {
            fprintf (stderr, "jack_client_open() failed, "
                             "status = 0x%2.0x\n", status);
            if (status & JackServerFailed) {
                fprintf (stderr, "Unable to connect to JACK server\n");
            }
            exit (1);
        }
        srate = jack_get_sample_rate (client);
        FPP = jack_get_buffer_size(client);
        PPS = srate/FPP;
        qDebug() << "srate = " << srate << "FPP = " << FPP;
        if (status & JackServerStarted) {
            qDebug() << "JACK server started";
        }
        if (status & JackNameNotUnique) {
            client_name = jack_get_client_name(client);
            fprintf (stderr, "unique name `%s' assigned\n", client_name);
        }

        jack_set_xrun_callback(client,xrun_callback, 0);

        /* tell the JACK server to call `process()' whenever
           there is work to be done.
        */

        jack_set_process_callback (client, process, 0);

        /* tell the JACK server to call `jack_shutdown()' if
           it ever shuts down, either entirely, or if it
           just decides to stop calling us.
        */

        jack_on_shutdown (client, jack_shutdown, 0);

        /* display the current sample rate.
         */

        fprintf (stderr,"engine sample rate: %" PRIu32 "\n",
                 jack_get_sample_rate (client));

        /* create two ports */

        input_port = jack_port_register (client, "input",
                                         JACK_DEFAULT_AUDIO_TYPE,
                                         JackPortIsInput, 0);
        output_port = jack_port_register (client, "output",
                                          JACK_DEFAULT_AUDIO_TYPE,
                                          JackPortIsOutput, 0);

        if ((input_port == NULL) || (output_port == NULL)) {
            fprintf(stderr, "no more JACK ports available\n");
            exit (1);
        }

        /* Tell the JACK server that we are ready to roll.  Our
         * process() callback will start running now. */

        if (jack_activate (client)) {
            fprintf (stderr, "cannot activate client");
            exit (1);
        }
        //        qDebug() << "activated";

        /* Connect the ports.  You can't do this before the client is
         * activated, because we can't make connections to clients
         * that aren't running.  Note the confusing (but necessary)
         * orientation of the driver backend ports: playback ports are
         * "input" to the backend, and capture ports are "output" from
         * it.
         */

        //        ports = jack_get_ports (client, NULL, NULL,
        //                    JackPortIsPhysical|JackPortIsOutput);
        //        if (ports == NULL) {
        //            fprintf(stderr, "no physical capture ports\n");
        //            exit (1);
        //        }

        //        if (jack_connect (client, ports[0], jack_port_name (input_port))) {
        //            fprintf (stderr, "cannot connect input ports\n");
        //        }

        //        free (ports);

        //        ports = jack_get_ports (client, NULL, NULL,
        //                    JackPortIsPhysical|JackPortIsInput);
        //        if (ports == NULL) {
        //            fprintf(stderr, "no physical playback ports\n");
        //            exit (1);
        //        }

        //        if (jack_connect (client, jack_port_name (output_port), ports[0])) {
        //            fprintf (stderr, "cannot connect output ports\n");
        //        }

        //        free (ports);

        /* keep running until stopped by the user */

        //        sleep (-1);

        /* this is never reached but if the program
           had some other way to exit besides being killed,
           they would be important to call.
        */

        //        jack_client_close (client);
        //        exit (0);
    }

    return a.exec();
}

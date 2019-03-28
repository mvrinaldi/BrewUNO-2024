import React, { Component } from 'react';
import SectionContent from '../components/SectionContent';
import MashSettings from './MashSettings';
import BoilSettings from './BoilSettings';
import Button from '@material-ui/core/Button';
import { withStyles } from '@material-ui/core/styles';
import { withNotifier } from '../components/SnackbarNotification';
import LinearProgress from '@material-ui/core/LinearProgress';
import ConfirmDialog from '../components/ConfirmDialog';
import { GET_ACTIVE_STATUS, START_BREW, NEXT_STEP_BREW, STOP_BREW, RESUME_BREW, ExecuteRestCall } from '../constants/Endpoints';

import ResponsiveContainer from 'recharts/lib/component/ResponsiveContainer';
import LineChart from 'recharts/lib/chart/LineChart';
import Line from 'recharts/lib/cartesian/Line';
import XAxis from 'recharts/lib/cartesian/XAxis';
import YAxis from 'recharts/lib/cartesian/YAxis';
import CartesianGrid from 'recharts/lib/cartesian/CartesianGrid';
import Tooltip from 'recharts/lib/component/Tooltip';
import Legend from 'recharts/lib/component/Legend';

import List from '@material-ui/core/List';
import ListItem from '@material-ui/core/ListItem';
import ListItemIcon from '@material-ui/core/ListItemIcon';
import ListItemText from '@material-ui/core/ListItemText';

const styles = theme => ({
  button: {
    margin: theme.spacing.unit,
  },
  input: {
    display: 'none',
  },
});

let interval, timerProgress;

class Brew extends Component {
  constructor(props) {
    super(props)
    this.getStatus();
    this.state = {
      status: {},
      data: [],
      fetched: false,
      progressCompleted: 0,
      confirmDialogOpen: false
    }

    interval = setInterval(() => {
      this.getStatus();
    }, 5000);

    timerProgress = setInterval(() => {
      this.brewProgress();
    }, 1000);
  }

  updateStatus() {
    if (this.state.status.active_step > 0 && this.state.status.brew_started == 1) {
      if (!this.state.fetched) {
        try {
          var json = eval("[" + this.state.status.temperatures + "]")
          var firstValues = [];
          json.forEach(element => {
            firstValues.push({ name: '', Target: element.t, Current: element.c })
          });
          this.setState({
            data: firstValues,
            fetched: true
          })
        }
        catch (e) {
          this.props.raiseNotification("Invalid temperatures json result");
          this.setState({
            fetched: true
          })
        }
      }
      else {
        if (this.state.data.length >= 100) {
          var array = this.state.data
          array.splice(0, 1);
          this.setState({ data: array });
        }
        var now = this.getDateTime(this.state.status.time_now);
        var time = now.getHours() + ":" + now.getMinutes() + ":" + now.getSeconds()
        this.setState({
          data: [...this.state.data, { name: time, Target: this.state.status.target_temperature, Current: this.state.status.temperature, PWM: this.state.status.pwm / 100 }]
        })
      }
    }
  }

  getStatus() {
    ExecuteRestCall(GET_ACTIVE_STATUS, 'GET', (json) => {
      if (json != null && json != undefined && json != '')
        this.setState({ status: json }, this.updateStatus)
    }, null, this.props)
  }

  getActiveStep() {
    switch (this.state.status.active_step) {
      case 1:
        return "Mash"
      case 2:
        return "Boil"
      default:
        return "Stopped"
    }
  }

  getDateTime(seconds) {
    var date = new Date(0);
    date.setUTCSeconds(seconds);
    return date;
  }

  brewProgress() {
    if (this.state.status.brew_started != 1)
      return;

    var dateEntered = this.getDateTime(this.state.status.end_time);
    var now = new Date();
    var difference = dateEntered.getTime() - now.getTime();

    if (difference <= 0) {
      this.setState({
        countdown: '00:00:00',
        progressCompleted: 100
      })
    } else {
      var seconds = Math.floor(difference / 1000);
      var minutes = Math.floor(seconds / 60);
      var hours = Math.floor(minutes / 60);
      var days = Math.floor(hours / 24);

      hours %= 24;
      minutes %= 60;
      seconds %= 60;

      this.setState({
        countdown: this.pad(hours, 2) + ':' + this.pad(minutes, 2) + ':' + this.pad(seconds, 2),
        progressCompleted: Math.round(((now - this.getDateTime(this.state.status.start_time)) / (this.getDateTime(this.state.status.end_time) - this.getDateTime(this.state.status.start_time))) * 100)
      })
    }
  }

  pad(n, width, z) {
    z = z || '0';
    n = n + '';
    return n.length >= width ? n : new Array(width - n.length + 1).join(z) + n;
  }

  componentWillUnmount() {
    clearInterval(interval);
  }

  render() {
    const { classes } = this.props;
    return (
      <SectionContent title="Brew">
        {this.state.status.active_step === 0 && this.state.status.brew_started === 0 ?
          <Button variant="contained" color="primary" className={classes.button}
            onClick={() => {
              this.setState({
                confirmDialogOpen: true,
                confirmDialogMessage: 'Do you want start brew?',
                confirmAction: (confirm) => {
                  if (confirm)
                    ExecuteRestCall(START_BREW, 'POST', (json) => { this.setState({ status: json, data: [] }) }, null, this.props)
                  this.setState({ confirmDialogOpen: false })
                }
              });
            }
            }
          >
            Start
      </Button> : null}

        {this.state.status.active_step > 0 && this.state.status.brew_started === 0 ?
          <Button variant="contained" color="primary" className={classes.button}
            onClick={() => {
              this.setState({
                confirmDialogOpen: true,
                confirmDialogMessage: 'Do you want resume brew?',
                confirmAction: (confirm) => {
                  if (confirm)
                    ExecuteRestCall(RESUME_BREW, 'POST', (json) => { this.setState({ status: json }) }, null, this.props)
                  this.setState({ confirmDialogOpen: false })
                }
              });
            }
            }
          >
            Resume
      </Button> : null}

        {this.state.status.active_step > 0 ?
          <Button variant="contained" color="secondary" className={classes.button}
            onClick={() => {
              this.setState({
                confirmDialogOpen: true,
                confirmDialogMessage: 'Do you want stop brew?',
                confirmAction: (confirm) => {
                  if (confirm)
                    ExecuteRestCall(STOP_BREW, 'POST', (json) => { this.setState({ data: [], status: json }) }, null, this.props)
                  this.setState({ confirmDialogOpen: false })
                }
              });
            }
            }
          >
            Stop
      </Button> : null}

        {this.state.status.active_step === 1 && this.state.status.brew_started === 1 ?
          <Button variant="contained" color="secondary" className={classes.button}
            onClick={() => {
              this.setState({
                confirmDialogOpen: true,
                confirmDialogMessage: 'Do you want skip the current step?',
                confirmAction: (confirm) => {
                  if (confirm)
                    ExecuteRestCall(NEXT_STEP_BREW, 'POST', (json) => { this.setState({ status: json }) }, null, this.props)
                  this.setState({ confirmDialogOpen: false })
                }
              });
            }
            }
          >
            Next Step
      </Button> : null}

        <ResponsiveContainer width="100%" height={320} >
          <LineChart data={this.state.data} margin={{ top: 5, right: 20, bottom: 5, left: 0 }}>
            <XAxis dataKey="name" />
            <YAxis yAxisId="left" />
            <YAxis yAxisId="right" orientation="right" />
            <CartesianGrid vertical={false} strokeDasharray="3 3" />
            <Tooltip />
            <Legend />
            <Line type="monotone" yAxisId="left" dataKey="Target" stroke="#82ca9d" dot={null} />
            <Line type="monotone" yAxisId="left" dataKey="Current" stroke="#8884d8" dot={null} activeDot={{ r: 10 }} />
            <Line type="monotone" yAxisId="left" dataKey="PWM" stroke="#FF0000" dot={null} />
          </LineChart>
        </ResponsiveContainer>

        <LinearProgress variant="determinate" value={this.state.progressCompleted} />
        <SectionContent title="Status">
          <List component="nav">
            <ListItem button>
              <ListItemText primary="Active Step" secondary={this.getActiveStep()} />
            </ListItem>
            <ListItem button>
              <ListItemText primary="Boil time" secondary={this.state.status.boil_time} />
            </ListItem>
            <ListItem button>
              <ListItemText primary="Start of step" secondary={
                this.state.status.start_time > 0
                  ? this.getDateTime(this.state.status.start_time).toLocaleString()
                  : null
              } />
            </ListItem>
            <ListItem button>
              <ListItemText primary="End of step" secondary={
                this.state.status.end_time > 0
                  ? this.getDateTime(this.state.status.end_time).toLocaleString()
                  : null
              } />
            </ListItem>
            <ListItem button>
              <ListItemText primary="Countdown" secondary={this.state.countdown} />
            </ListItem>
          </List>
        </SectionContent>

        <ConfirmDialog
          confirmAction={this.state.confirmAction}
          confirmDialogOpen={this.state.confirmDialogOpen}
          confirmDialogMessage={this.state.confirmDialogMessage}
        />

        <MashSettings listOnly={true} brewDay={true} selectedIndex={this.state.status.active_mash_step_index} />
        <BoilSettings listOnly={true} brewDay={true} selectedIndex={this.state.status.active_boil_step_index} />
      </SectionContent >
    )
  }
}

export default withNotifier(withStyles(styles)(Brew));
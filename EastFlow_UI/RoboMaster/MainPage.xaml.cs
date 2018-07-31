using Windows.UI;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Controls;
using Windows.ApplicationModel.DataTransfer;
//using static RoboMaster.CellModel;
using static Windows.UI.Colors;
//using static RoboMaster.CellModel.What;

using System;
using Windows.UI.ViewManagement;
//using Windows.ApplicationModel.Core;



namespace RoboMaster {
    public sealed partial class MainPage {
        private readonly Button[] _buttons = new Button[64];
        //private readonly CellModel _cells = new CellModel();
        private int[] colors = { 0, 0, 0, 0 };
        private bool readClip = false;

        private AcrylicBrush myWhite = new AcrylicBrush();
        private AcrylicBrush myRed = new AcrylicBrush();
        private AcrylicBrush myGreen = new AcrylicBrush();
        private AcrylicBrush myBlue = new AcrylicBrush();
        private AcrylicBrush myYellow = new AcrylicBrush();

        public MainPage() {
            InitializeComponent();
            
            ApplicationViewTitleBar titleBar = ApplicationView.GetForCurrentView().TitleBar;
            titleBar.ButtonBackgroundColor = Colors.Transparent;
            titleBar.ButtonInactiveBackgroundColor = Colors.Transparent;

            myWhite.BackgroundSource = Windows.UI.Xaml.Media.AcrylicBackgroundSource.HostBackdrop;
            myWhite.TintColor = Color.FromArgb(240, 255, 255, 255);
            myWhite.FallbackColor = Color.FromArgb(200, 220, 220, 220);
            myWhite.TintOpacity = 0.8;

            myRed.BackgroundSource = Windows.UI.Xaml.Media.AcrylicBackgroundSource.HostBackdrop;
            myRed.TintColor = Red;
            myRed.FallbackColor = OrangeRed;
            myRed.TintOpacity = 0.5;

            myGreen.BackgroundSource = Windows.UI.Xaml.Media.AcrylicBackgroundSource.HostBackdrop;
            myGreen.TintColor = ForestGreen;
            myGreen.FallbackColor = ForestGreen;
            myGreen.TintOpacity = 0.5;

            myBlue.BackgroundSource = Windows.UI.Xaml.Media.AcrylicBackgroundSource.HostBackdrop;
            myBlue.TintColor = RoyalBlue;
            myBlue.FallbackColor = RoyalBlue;
            myBlue.TintOpacity = 0.5;

            myYellow.BackgroundSource = Windows.UI.Xaml.Media.AcrylicBackgroundSource.HostBackdrop;
            myYellow.TintColor = Gold;
            myYellow.FallbackColor = Gold;
            myYellow.TintOpacity = 0.5;


            for (var i = 0; i < 64; ++i) {
                var position = (i / 8, i % 8);
                var button = DefaultButton();
                button.Tag = position;
                button.Background = myWhite;
                button.Click += ObjectClick;

                MainGrid.Children.Add(button);
                Grid.SetRow(button, position.Item1 + 1);
                Grid.SetColumn(button, position.Item2);

                _buttons[i] = button;
            }
            Mode = myRed;
            Title.Foreground = new SolidColorBrush(Black);
        }


        private static Button DefaultButton()
            => new Button
            {
                FontFamily = new FontFamily("Segoe UI"),
                FontSize = 20,
                Margin = new Thickness(4),
                Background = new AcrylicBrush(),
                HorizontalAlignment = HorizontalAlignment.Stretch,
                VerticalAlignment = VerticalAlignment.Stretch,
            };

        void Do(DispatchedHandler action) {
            var act = Dispatcher.RunAsync(CoreDispatcherPriority.Low, action);
        }

        private Brush _mode;

        private Brush Mode {
            get => _mode;
            set {
                Do(() => Title.Foreground = value);
                _mode = value;
                if (_mode == myWhite)
                    for (int i = 0; i < 4; i++)
                        colors[i] = -10;

            }
        }

        private void ObjectClick(object sender, RoutedEventArgs e) {
            if (colors[0] == 0)
                TimeCalc();
            var button = (Button) sender;
            var position = ((int, int)) button.Tag;
             
            if (_mode != myWhite)
            {
                Title.Foreground = _mode;
                if (_mode == myRed)
                {
                    colors[0]++;
                    Title.Text = "EastFlow";
                    button.Content = "🕍";
                    if (colors[0] > 4)
                    {
                        _mode = myGreen;
                    }
                }
                if (_mode == myGreen)
                {
                    colors[1]++;
                    Title.Text = "EastFlow";
                    button.Content = "🏯";
                    if (colors[1] > 4)
                    {
                        _mode = myBlue;
                    }
                }
                if (_mode == myBlue)
                {
                    colors[2]++;
                    Title.Text = "EastFlow" ;
                    button.Content = "🏰";
                    if (colors[2] > 4)
                    {
                        _mode = myYellow;
                    }
                }
                if (_mode == myYellow)
                {
                    colors[3]++;
                    Title.Text = "EastFlow" ;
                    button.Content = "🏛";
                    if (colors[3] > 3)
                    {
                        GenerateClip();
                    }
                }

                button.Background = _mode;
            }
            else
            {
                button.Content = "";
                Title.Text = "EastFlow";
                Title.Foreground = new SolidColorBrush(Black);
                button.Background = myWhite;
            }
            //_cells[position.Item1, position.Item2] = (_mode, Castle);
            //if (_cells.Sum(cell => cell == (_mode, Castle)) >= 4) Mode = Default.color;
        }

        private void AddRed_Click(object sender, RoutedEventArgs e) => Mode = myRed;
        private void AddYellow_Click(object sender, RoutedEventArgs e) => Mode = myYellow;
        private void AddGreen_Click(object sender, RoutedEventArgs e) => Mode = myGreen;
        private void AddBlue_Click(object sender, RoutedEventArgs e) => Mode = myBlue;
        private void AddGray_Click(object sender, RoutedEventArgs e) => Mode = myWhite;

        private void Reset_Click(object sender, RoutedEventArgs e) {
            //_cells.ClearAll();
            Mode = myRed;
            Title.Foreground = new SolidColorBrush(Black);
            Title.Text = "EastFlow";
            colors[0] = 0;
            colors[1] = 0;
            colors[2] = 0;
            colors[3] = 0;
            foreach (var button in _buttons)
            {
                button.Background = myWhite;
                button.Content = "";
            }

        }

        private void Generate(object sender, RoutedEventArgs e)
        {
            GenerateClip();
        }


        private async void Load(object sender, RoutedEventArgs e)
        {
            char[] block = new char[64];
            string readstr = "", str = "";
            DataPackageView con = Clipboard.GetContent();
            if (con.Contains(StandardDataFormats.Text))
            {
                readstr = await con.GetTextAsync();
            }
            bool city = readstr.Contains("W");
            if (readstr.Length >= 8 * 8)
            {
                for (int i = 0; i < 8; i++)
                    str += readstr.Substring(10 * i, 8);

                block = str.ToCharArray();

                for (int i = 0; i < 64; i++)
                {
                    if (_buttons[i].Content == null || _buttons[i].Content == "")
                        switch (block[i])
                        {
                            case 'R':
                                _buttons[i].Background = myRed;
                                if (city)
                                    _buttons[i].Content = "🕍";
                                break;
                            case 'G':
                                _buttons[i].Background = myGreen;
                                if (city)
                                    _buttons[i].Content = "🏯";
                                break;
                            case 'B':
                                _buttons[i].Background = myBlue;
                                if (city)
                                    _buttons[i].Content = "🏰";
                                break;
                            case 'Y':
                                _buttons[i].Background = myYellow;
                                if (city)
                                    _buttons[i].Content = "🏛";
                                break;
                            case 'W':
                                _buttons[i].Background = myWhite;
                                break;
                        }
                }
            }
        }

        private async void GenerateClip()
        {
            string text = "";
            for (int i = 0; i < 64; i++)
            {
                if (_buttons[i].Content == "🏰")
                    text += "B";
                else if (_buttons[i].Content == "🏯")
                    text += "G";
                else if (_buttons[i].Content == "🏛")
                    text += "Y";
                else if (_buttons[i].Content == "🕍")
                    text += "R";
                else
                    text += "W";

                if ((i + 1) % 8 == 0)
                    text += "\r\n";
            }
            
            DataPackage dataPackage = new DataPackage();
            dataPackage.SetText(text);
            Clipboard.SetContent(dataPackage);
            readClip = true;
            Title.Foreground = new SolidColorBrush(Black);
            Title.Text = "COPIED";
        }
        

        private async void TimeCalc()
        {
            int time = 0;
            char[] block = new char[64];
            string readstr = "", str = "";

            while (true)
            {
                await System.Threading.Tasks.Task.Run(() =>
                {
                    System.Threading.Thread.Sleep(1000);
                });

                time++;
                Time.Text = time + "s";

                /* if(readClip)
                {
                    Load(null,null);
                } */

            }
        }



        private void Title_SelectionChanged(object sender, RoutedEventArgs e)
        {

        }
    }
}
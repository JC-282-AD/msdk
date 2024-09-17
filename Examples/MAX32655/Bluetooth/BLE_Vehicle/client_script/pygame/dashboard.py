import pygame
import sys

class VehicleDashboard:
    def __init__(self):
        # Initialize Pygame
        pygame.init()

        # Set up display
        self.WIDTH, self.HEIGHT = 800, 600
        self.screen = pygame.display.set_mode((self.WIDTH, self.HEIGHT))
        pygame.display.set_caption("Vehicle Dashboard")

        # Colors
        self.BLACK = (18, 18, 18)
        self.WHITE = (255, 255, 255)
        self.CYAN = (0, 255, 255)
        self.ORANGE = (255, 165, 0)
        self.GREEN = (0, 255, 0)
        self.BLUE = (0, 0, 255)

        # Font
        self.font = pygame.font.SysFont('Arial', 30)
        self.big_font = pygame.font.SysFont('Arial', 40)

        # Initial values
        self.speed = 0
        self.accx = 0
        self.accy = 0
        self.accz = 1
        self.batt = 100
        self.distance = 100

    def initialize(self):
        self.screen.fill(self.BLACK)
        connecting_text = self.big_font.render("Connecting to the Vehicle...", True, self.WHITE)
        self.screen.blit(connecting_text, (self.WIDTH // 2 - connecting_text.get_width() // 2, self.HEIGHT // 2 - connecting_text.get_height() // 2))
        pygame.display.flip()

    def finalize(self):
        self.screen.fill(self.BLACK)
        connecting_text = self.big_font.render("Disconnecting...", True, self.WHITE)
        self.screen.blit(connecting_text, (self.WIDTH // 2 - connecting_text.get_width() // 2, self.HEIGHT // 2 - connecting_text.get_height() // 2))
        pygame.display.flip()

    def draw_dashboard(self):
        self.screen.fill(self.BLACK)

        # Speedometer
        speed_label = self.font.render("Speed", True, self.WHITE)
        speed_value = self.big_font.render(f"{self.speed:.1f} cm/s", True, self.CYAN)
        self.screen.blit(speed_label, (150, 100))
        self.screen.blit(speed_value, (150, 150))

        # Tachometer
        acceleration_label = self.font.render("Acceleration", True, self.WHITE)
        acceleration_valuex = self.big_font.render(f"{self.accx:.1f} G", True,  self.ORANGE)
        acceleration_valuey = self.big_font.render(f"{self.accy:.1f} G", True,  self.ORANGE)
        acceleration_valuez = self.big_font.render(f"{self.accz:.1f} G", True,  self.ORANGE)
        self.screen.blit(acceleration_label, (450, 100))
        self.screen.blit(acceleration_valuex, (450, 150))
        self.screen.blit(acceleration_valuey, (450, 200))
        self.screen.blit(acceleration_valuez, (450, 250))

      
        fuel_label = self.font.render("Battery", True, self.WHITE)
        fuel_value = self.big_font.render(f"{self.batt}", True, self.GREEN)
        self.screen.blit(fuel_label, (150, 300))
        self.screen.blit(fuel_value, (150, 350))

        
        battery_label = self.font.render("Distance", True, self.WHITE)
        battery_value = self.big_font.render(f"{self.distance}", True, self.BLUE)
        self.screen.blit(battery_label, (450, 300))
        self.screen.blit(battery_value, (450, 350))

        # Navigation Button (Placeholder)
        #nav_button = self.font.render("Show Navigation", True, self.WHITE)
        #pygame.draw.rect(self.screen, self.WHITE, (300, 450, 200, 50))
        #self.screen.blit(nav_button, (320, 460))

        pygame.display.flip()



if __name__ == "__main__":
    dashboard = VehicleDashboard()

